# ORM vs SQLite3 Performance Overhead Detailed Analysis

## Test Data

Source: `bench/data/comprehensive_results.json` (Memory Database)

### Insert

| Batch | SQLite3 (ns) | ORM (ns) | Per-item SQLite3 (ns) | Per-item ORM (ns) | Overhead |
|-------|-------------|---------|----------------------|------------------|----------|
| 10 | 8,743 | 12,992 | 874 | 1,299 | 48.6% |
| 100 | 61,910 | 89,406 | 619 | 894 | 44.4% |
| 1,000 | 605,974 | 881,697 | 606 | 882 | 45.5% |
| 10,000 | 6,048,164 | 8,710,325 | 605 | 871 | 44.0% |

### Query

| Batch | SQLite3 (ns) | ORM (ns) | Per-item SQLite3 (ns) | Per-item ORM (ns) | Overhead |
|-------|-------------|---------|----------------------|------------------|----------|
| 10 | 3,693 | 5,408 | 369 | 541 | 46.4% |
| 100 | 24,754 | 32,440 | 248 | 324 | 31.1% |
| 1,000 | 212,483 | 301,065 | 212 | 301 | 41.7% |
| 10,000 | 2,413,351 | 2,996,489 | 241 | 300 | 24.2% |

### Update

| Batch | SQLite3 (ns) | ORM (ns) | Per-item SQLite3 (ns) | Per-item ORM (ns) | Overhead |
|-------|-------------|---------|----------------------|------------------|----------|
| 10 | 5,033 | 8,723 | 503 | 872 | 73.3% |
| 100 | 39,567 | 67,655 | 396 | 677 | 71.0% |
| 1,000 | 415,466 | 684,590 | 415 | 685 | 64.8% |

### Delete

| Batch | SQLite3 (ns) | ORM (ns) | Per-item SQLite3 (ns) | Per-item ORM (ns) | Overhead |
|-------|-------------|---------|----------------------|------------------|----------|
| 10 | 2,486 | 3,976 | 249 | 398 | 59.9% |
| 100 | 3,476 | 5,025 | 35 | 50 | 44.6% |
| 1,000 | 13,701 | 12,972 | 14 | 13 | -5.3% |

---

## Overhead Source Analysis

### 1. Query Path: Result Set Mapping

**Hot path**: `ResultSetMapper<Table>::map_row()` in `result_set_mapper.hpp`

```cpp
// Simplified hot path (called once per row)
template <typename Table>
Table ResultSetMapper<Table>::map_row(Statement& stmt) {
    Table obj;
    // Field-to-column mapping (pre-computed at init_mapper)
    for (size_t i = 0; i < N; ++i) {
        int col_idx = m_field_to_col_map[i];
        if (col_idx >= 0) {
            // Compile-time dispatch via Visitor/Accessor
            dsl::Visitor::foreach<Table>([&](auto field) {
                using Acc = dsl::FieldAccessor<decltype(field)>;
                auto& val = Acc::get(obj);
                val = stmt.get_column<Acc::member_type>(col_idx);
            });
        }
    }
    return obj;
}
```

**The `init_mapper` cold path** (called once per query):

```cpp
void init_mapper(Statement& stmt) {
    int col_count = stmt.column_count();
    for (int i = 0; i < N; ++i) {
        int col = -1;
        // DJB2 hash lookup (integer comparison, no string ops in hot path)
        for (int j = 0; j < col_count; ++j) {
            if (hashFn(stmt.column_name_view(j)) == field_hashes[i]) {
                col = j; break;
            }
        }
        m_field_to_col_map[i] = col;
    }
    // Unknown column check (cold path, not in map_row)
}
```

**Overhead budget breakdown for Query**:

| Step | Code Location | Cost | Notes |
|------|-------------|------|-------|
| Statement cache lookup | `sqlite3_connection.cpp:37-48` | Medium | `unordered_map::find()` + `reset()` + `clear_bindings()` |
| Init mapper | `result_set_mapper.hpp` | One-time | DJB2 hash + linear scan (cold path) |
| Column extraction x N | `map_row` | Low per field | `stmt.get_column<T>(col_idx)` |
| Struct field assignment | Visitor/Accessor dispatch | Negligible | Compile-time inlined, zero indirect call overhead |

### 2. Why Update Has the Highest Overhead (65-73%)

Update operations share the same framework overhead as Insert/Query but also:
- More complex SQL (`SET name = ? WHERE id = ?`) vs Insert's simpler statement
- WHERE clause evaluation inside SQLite adds per-row cost that is comparable to the ORM's fixed overhead
- Result: the ORM's constant overhead is a larger fraction of the total

### 3. Why Delete Is Faster at Scale (-5.3% at 1000)

| Batch | ORM faster than SQLite3 | Reason |
|-------|------------------------|--------|
| 1000 | -5.3% | ORM's `prepare_cached` cache reuse + SQLite WAL mode batching |

Delete at batch=1000 benefits from:
- Statement cache hit (no re-prepare cost)
- WAL journal mode (reduced fsync)
- Per-row ORM overhead is dominated by SQLite's internal processing at scale

---

## Optimizations Applied

### Round 1: Compile-time String Hashing

**File**: `include/ess/orm/core/result_set_mapper.hpp`

- SchemaMapper uses `consteval` DJB2 hash computed at compile time from sorted field names
- `init_mapper()` compares integer hashes instead of `strcmp`/`std::string::operator==`
- `column_name_view()` returns `string_view` to avoid `std::string` heap allocation from `sqlite3_column_name()`

### Round 2: Visitor/Accessor Pattern

**Files**: `dsl/visitor.hpp`, `dsl/accessor.hpp`

- Replaced `FillFunc` function pointer array + virtual dispatch with `dsl::Visitor::foreach` + `dsl::FieldAccessor::get`
- Compile-time type resolution: no indirect call overhead per field
- `std::array<int, N>` stack-allocated field-to-column map replaces heap-allocated `std::vector`

### Round 3: Cold/Hot Path Separation

- Moved "unknown column" exception from `map_row()` (hot, per-row) to `init_mapper()` (cold, once per query)
- Field-to-column mapping pre-computation means `map_row()` does only integer lookups and value extraction

---

## ORM Core Architecture

### Call Chain

```
query<User, "INSERT INTO users ...">(params...)
    |
    runtime.hpp -> query_impl
    |
    impl.hpp -> insert_impl / select_impl / update_impl / delete_impl
    |
    sqlite3_statement.cpp -> stmt.next()
```

### Key Files

| File | Function |
|------|----------|
| `core/runtime.hpp` | query entry (template) |
| `core/impl.hpp` | query_impl implementation |
| `core/result_set_mapper.hpp` | Result set mapping (optimized) |
| `core/cache.hpp` | SQL statement cache |
| `core/statement.hpp` | Abstract statement interface |
| `sqlite3_statement.cpp` | SQLite3 connection + prepare_cached |
| `dsl/visitor.hpp` | Compile-time field visitor |
| `dsl/accessor.hpp` | Compile-time field accessor |

---

## Performance Optimization Suggestions

### Applied

1. **SQL statement cache** - `prepare_cached()` uses `unordered_map` for pre-compiled statements
2. **Compile-time field mapping** - `SchemaMapper` with `consteval` DJB2 hash map, zero runtime string comparison
3. **Visitor/Accessor dispatch** - Compile-time inlined field access, no function pointer or virtual call overhead
4. **Cold/hot path separation** - Validation errors in `init_mapper()`, not `map_row()`
5. **WAL mode** - Better concurrent write performance

### Future Directions

1. **Connection pool warmup** - Pre-create connections at startup
2. **Batch INSERT** - Multi-row insert syntax (`INSERT INTO t VALUES (...), (...), ...`)
3. **Skip statement cache for simple SQL** - Direct `sqlite3_prepare` for trivial queries
4. **move semantics** - Zero-copy row extraction
