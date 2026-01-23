# Ess ORM 设计建议与实现指南

## 目录

1. [数据库 API 与 AS 别名支持](#1-数据库-api-与-as-别名支持)
2. [多 Dialect 支持方案](#2-多-dialect-支持方案)
3. [当前库效率评估与优化](#3-当前库效率评估与优化)
4. [配置系统设计](#4-配置系统设计)
5. [事务与查询处理](#5-事务与查询处理)
6. [缓存系统设计](#6-缓存系统设计)
7. [SQL 语法检测](#7-sql-语法检测)
8. [用户接入与编译配置](#8-用户接入与编译配置)
9. [额外便利功能](#9-额外便利功能)

---

## 1. 数据库 API 与 AS 别名支持

### 1.1 AS 别名的 API 支持

**是的，所有主流数据库 API 都完全支持 AS 别名：**

| 数据库         | API 函数                         | 返回值        | 说明                           |
| -------------- | -------------------------------- | ------------- | ------------------------------ |
| **SQLite3**    | `sqlite3_column_name(stmt, idx)` | `const char*` | 返回 AS 指定的别名，或原始列名 |
| **MySQL**      | `mysql_fetch_field()->name`      | `char*`       | 同上                           |
| **PostgreSQL** | `PQfname(res, idx)`              | `char*`       | 同上                           |

### 1.2 各数据库 API 简介

#### SQLite3 (已使用)

```cpp
// 连接管理
sqlite3_open() / sqlite3_close()

// 语句管理
sqlite3_prepare_v2()  // 编译 SQL
sqlite3_step()         // 执行/迭代
sqlite3_finalize()     // 清理

// 结果访问
sqlite3_column_count()      // 列数
sqlite3_column_name()       // 列名（支持 AS 别名）
sqlite3_column_type()       // 类型
sqlite3_column_int() / _int64() / _double() / _text()

// 参数绑定
sqlite3_bind_int() / _text() / _null()
```

#### MySQL C API

```cpp
// 连接管理
mysql_init() / mysql_real_connect() / mysql_close()

// 查询执行
mysql_query() / mysql_real_query()
mysql_store_result() / mysql_use_result()

// 结果访问
mysql_num_fields()           // 列数
mysql_fetch_field()          // 获取字段信息
// mysql_fetch_field()->name 就是 AS 别名
mysql_fetch_row()           // 获取一行
mysql_fetch_lengths()       // 每列长度

// 类型转换
字符串形式返回所有值，通过 atoi/atof/atoll 转换
```

#### PostgreSQL (libpq)

```cpp
// 连接管理
PQconnectdb() / PQfinish()

// 查询执行
PQexec() / PQexecParams()
PQprepare() / PQexecPrepared()

// 结果访问
PQnfields()                 // 列数
PQfname()                   // 列名（支持 AS 别名）
PQftype()                   // 类型 OID
PQgetvalue()                // 获取值（字符串形式）
PQgetisnull()               // 是否为 NULL
PQntuples()                 // 行数
```

### 1.3 多表查询映射方案

当前 `ResultSetMapper` 已是按列名匹配，配合 AS 别名即可实现多表查询映射：

```sql
SELECT
  g.id AS id,
  g.title AS title,
  o.total AS total
FROM goods g
JOIN orders o ON g.id = o.goods_id
```

配合 DTO：

```cpp
struct GoodsOrderDTO {
  long long id;
  std::string title;
  float total;
  using Schema = Schema<"",
    Field<"id", &GoodsOrderDTO::id>,
    Field<"title", &GoodsOrderDTO::title>,
    Field<"total", &GoodsOrderDTO::total>
  >;
};
```

---

## 2. 多 Dialect 支持方案

### 方案对比

| 方案                     | 描述                   | 优点                         | 缺点                     | 适用场景           |
| ------------------------ | ---------------------- | ---------------------------- | ------------------------ | ------------------ |
| **A: 编译期类型擦除**    | 基于 Traits 的模板特化 | 零运行时开销、编译期类型检查 | 不能运行时切换           | 高性能应用（推荐） |
| **B: 虚函数多态**        | 接口 + 具体实现        | 运行时切换灵活、代码清晰     | 虚函数调用开销、无法内联 | 需要运行时切换     |
| **C: CRTP**              | 奇异递归模板模式       | 零运行时开销、代码复用       | 不能运行时切换           | 零开销抽象         |
| **D: 类型擦除混合**      | 虚接口 + 模板特化      | 外部 API 统一                | 每个函数一次虚调用       | 通用型库           |
| **E: 编译期 + 配置文件** | 宏/模板参数选择        | 性能最优、环境适配性强       | 需要重新编译             | 生产环境（推荐）   |

### 推荐方案：A + E 组合

```cpp
// 1. Dialect Traits
template <typename Dialect> struct DialectTraits;

template <> struct DialectTraits<dialect::Sqlite3> {
  using HandleType = sqlite3*;
  static constexpr const char* name = "sqlite3";

  template <typename T>
  static T get_column(HandleType stmt, int idx);

  static std::string get_column_name(HandleType stmt, int idx);
  static int column_count(HandleType stmt);
};

// 2. 泛型 Connection 基类
template <typename Dialect>
class ConnectionBase {
protected:
  using Handle = typename DialectTraits<Dialect>::HandleType;
  Handle m_handle;
public:
  template <typename T>
  T get_column(int idx) const {
    return DialectTraits<Dialect>::template get_column<T>(m_handle, idx);
  }
};

// 3. 编译期选择
template <typename Dialect = DefaultDialect>
class ConnectionPool { ... };

// 4. 使用
ConnectionPool<dialect::Sqlite3> pool1;
ConnectionPool<dialect::Postgres> pool2;
```

---

## 3. 当前库效率评估与优化

### 效率评分

| 指标               | 当前状态  | 评分 | 说明                              |
| ------------------ | --------- | ---- | --------------------------------- |
| **编译期优化**     | ✅ 优秀   | 9/10 | 模板元编程、constexpr             |
| **连接池设计**     | ✅ 良好   | 7/10 | RAII、线程安全，但 `deque` 可优化 |
| **Statement 缓存** | ✅ 良好   | 8/10 | 减少重复编译                      |
| **内存分配**       | ⚠️ 一般   | 6/10 | 多处动态分配，可用 `pmr`          |
| **字符串处理**     | ⚠️ 可优化 | 6/10 | `SQLITE_TRANSIENT` 会复制         |
| **线程安全**       | ✅ 良好   | 8/10 | 正确使用同步原语                  |

### 具体优化建议

#### 1. Statement 缓存优化

```cpp
// 当前：connection.h:16
std::unordered_map<std::string, StatementPtr> m_stmt_cache{};

// 建议：使用 string_view 作为 key，避免字符串拷贝
std::unordered_map<std::string, StatementPtr,
                   std::hash<std::string_view>,
                   std::equal_to<>> m_stmt_cache{};
```

#### 2. 字符串绑定优化

```cpp
// 当前：statement.h:89-90
void bind_one(int index, std::string const &param) {
  sqlite3_bind_text(m_stmt.get(), index, param.c_str(), -1, SQLITE_TRANSIENT);
}

// 建议：使用自定义析构避免复制（如果参数生命周期足够长）
void bind_one(int index, std::string const &param) {
  // 假设 param 生命周期长于 statement 执行
  sqlite3_bind_text(m_stmt.get(), index, param.c_str(),
                    static_cast<int>(param.length()),
                    SQLITE_STATIC);
}
```

#### 3. 调试日志条件编译

```cpp
// 当前：statement.h:49-50
auto expanded = expanded_sql();
fmt::println("SQL: {}", expanded);

// 建议：仅 Debug 模式输出
#ifdef ORM_DEBUG_LOG
auto expanded = expanded_sql();
fmt::println("SQL: {}", expanded);
#endif
```

#### 4. 连接池容器优化

```cpp
// 当前：connection_pool.h:59
std::deque<std::shared_ptr<Connection>> m_pool;

// 建议：使用 vector + index 减少指针追逐
std::vector<std::shared_ptr<Connection>> m_pool;
size_t m_next{0};  // 轮询索引
```

#### 5. 映射初始化优化

```cpp
// 当前：result_set_mapper.hpp:55-56
inline static std::vector<int> m_col_to_field_map{};
inline static std::once_flag m_init_flag{};

// 问题：static 变量对多表查询不安全
// 建议：非 static，每次查询时初始化
std::vector<int> m_col_to_field_map;
```

### 性能关键点总结

| 优化点                   | 预期收益   | 优先级 |
| ------------------------ | ---------- | ------ |
| 移除调试日志             | ~10%       | 高     |
| 字符串绑定优化           | ~5-15%     | 中     |
| 连接池 vector 化         | ~5%        | 低     |
| Statement 缓存 hash 优化 | ~3%        | 低     |
| 映射初始化去 static      | 正确性修复 | 高     |

---

## 4. 配置系统设计

### 4.1 多数据库配置的 Trait 设计

```cpp
// 扩展 config/traits.hpp
namespace ess::orm::config::detail {

// 数据库配置 concept
template <typename T>
concept database_config = requires {
  { T::url } -> std::convertible_to<std::string_view>;
  { T::pool_size } -> std::convertible_to<std::size_t>;
  { T::dialect } -> dialect_type;
};

// 可选配置
template <typename T>
concept has_read_only = requires {
  { T::read_only } -> std::convertible_to<bool>;
};

template <typename T>
concept has_cache_enabled = requires {
  { T::cache_enabled } -> std::convertible_to<bool>;
};

} // detail

// 统一的数据库配置 Trait
template <typename DBConfig>
struct DatabaseConfigTrait {
  static constexpr std::string_view url = DBConfig::url;
  static constexpr std::size_t pool_size = DBConfig::pool_size;
  using dialect = DBConfig::dialect;

  // 默认值
  static constexpr bool read_only = [] {
    if constexpr (detail::has_read_only<DBConfig>) {
      return DBConfig::read_only;
    } else {
      return false;
    }
  }();

  static constexpr bool cache_enabled = [] {
    if constexpr (detail::has_cache_enabled<DBConfig>) {
      return DBConfig::cache_enabled;
    } else {
      return false;  // 默认关闭
    }
  }();
};
```

### 4.2 用户配置示例

```cpp
// ess_orm_user_config.hpp 中：

struct MainDB {
  static constexpr std::string_view url = "data/main.db";
  static constexpr std::size_t pool_size = 20;
  // 其他使用默认值
};

struct ReplicaDB {
  static constexpr std::string_view url = "data/replica.db";
  static constexpr std::size_t pool_size = 5;
  static constexpr bool read_only = true;
};

struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;
  using databases = std::tuple<MainDB, ReplicaDB>;
  using default_db = MainDB;  // 可选：指定默认数据库
};
```

---

## 5. 事务与查询处理

### 5.1 Context 单例设计

```cpp
// include/ess/orm/runtime.hpp
namespace ess::orm {

class Context {
private:
  // 类型擦除的连接池存储
  struct PoolConcept {
    virtual ~PoolConcept() = default;
  };

  template <typename DBConfig>
  struct PoolModel : PoolConcept {
    std::unique_ptr<ConnectionPool<typename DBConfig::dialect>> pool;

    PoolModel(std::string_view url, std::size_t size)
        : pool(std::make_unique<ConnectionPool<typename DBConfig::dialect>>(url, size)) {}
  };

  std::unordered_map<std::string, std::unique_ptr<PoolConcept>> m_pools;

  Context() = default;

public:
  // 单例
  static Context& instance() {
    static Context ctx;
    return ctx;
  }

  // 注册数据库（编译期调用）
  template <typename DBConfig>
  auto& register_db() {
    static constexpr auto& cfg = DatabaseConfigTrait<DBConfig>;
    std::string name = typeid(DBConfig).name();  // 或使用用户指定的 tag

    m_pools.emplace(name, std::make_unique<PoolModel<DBConfig>>(
        cfg.url, cfg.pool_size
    ));

    return *static_cast<PoolModel<DBConfig>*>(m_pools[name].get())->pool;
  }

  // 获取连接池
  template <typename DBConfig>
  auto& pool() {
    std::string name = typeid(DBConfig).name();
    auto it = m_pools.find(name);
    if (it == m_pools.end()) {
      throw std::runtime_error("Database not registered: " + name);
    }
    return *static_cast<PoolModel<DBConfig>*>(it->second.get())->pool;
  }
};
} // ess::orm
```

### 5.2 自动注册机制

```cpp
// 自动注册机制
template <typename DBConfig>
struct AutoRegister {
  AutoRegister() {
    Context::instance().register_db<DBConfig>();
  }
};

// 用户在配置中声明
template <typename... DBs>
struct AutoRegisterDatabases {
  AutoRegisterDatabases() {
    (AutoRegister<DBs>(), ...);
  }
};

// 配置扩展
template <typename T>
struct extract_databases {
  using type = std::tuple<>;
};

template <typename T>
  requires(requires { typename T::databases; })
struct extract_databases<T> {
  using type = typename T::databases;
};

using databases = typename extract_databases<config>::type;
using default_db = typename extract_default_db<config>::type;

// 全局自动注册器（自动调用）
inline auto global_auto_register = []() {
  return AutoRegisterDatabases<databases>{};
}();
```

### 5.3 事务设计

```cpp
// 隐式模式（推荐）
template <typename F>
auto transaction(F&& fn) {
  using DefaultDB = config::default_db;
  return transaction<DefaultDB>(std::forward<F>(fn));
}

// 指定数据库的事务
template <typename DBConfig, typename F>
auto transaction(F&& fn) {
  auto& pool = Context::instance().pool<DBConfig>();
  auto guard = pool.acquire();

  Transaction<typename DBConfig::dialect> tx(guard.shared());

  try {
    if constexpr (std::is_invocable_v<F, Transaction<typename DBConfig::dialect>&>) {
      std::invoke(fn, tx);  // 用户可以访问 tx 进行高级操作
    } else {
      std::invoke(fn);  // 简单模式
    }
    tx.commit();
    return true;
  } catch (...) {
    tx.rollback();
    throw;
  }
}
```

### 5.4 使用示例

```cpp
// 简单模式（单数据库）
transaction([] {
  query<Order, "UPDATE orders SET status = 1 WHERE id = ?">(1);
});

// 嵌套事务（各自独立）
transaction([] {
  query<Order, "UPDATE orders ...">();

  // 这是另一个数据库的独立事务
  transaction<ReplicaDB>([] {
    query<Analytics, "INSERT INTO events ...">();
  });
});

// 高级模式（访问 tx 对象）
transaction([](auto& tx) {
  auto result = query<Order, "SELECT ...">();

  // 手动控制事务
  if (some_condition) {
    tx.savepoint("sp1");
    // ...
    tx.release_savepoint("sp1");
  }
});
```

### 5.5 查询结果处理（函数式）

```cpp
template <typename DBConfig, typename Table, meta::FixedString SQL, typename... Args>
class QueryResult {
private:
  std::vector<Table> m_results;

public:
  QueryResult(std::vector<Table> results) : m_results(std::move(results)) {}

  // 函数式操作
  template <typename F>
  auto map(F&& fn) {
    std::vector<std::invoke_result_t<F, Table>> mapped;
    mapped.reserve(m_results.size());
    for (auto& item : m_results) {
      mapped.push_back(std::invoke(fn, item));
    }
    return QueryResult<DBConfig, Table, "", Args...>(std::move(mapped));
  }

  template <typename F>
  auto filter(F&& fn) {
    std::vector<Table> filtered;
    for (auto& item : m_results) {
      if (std::invoke(fn, item)) {
        filtered.push_back(item);
      }
    }
    return QueryResult<DBConfig, Table, "", Args...>(std::move(filtered));
  }

  template <typename T, typename F>
  T reduce(T init, F&& fn) {
    for (auto& item : m_results) {
      init = std::invoke(fn, init, item);
    }
    return init;
  }

  // 转换为标准容器
  std::vector<Table> to_vector() && { return std::move(m_results); }

  // 链式调用示例
  auto total = query<Order, "SELECT * FROM orders WHERE user_id = ?">(1)
    .filter([](auto& o) { return o.amount > 100; })
    .map([](auto& o) { return o.amount; })
    .reduce(0, [](auto acc, auto val) { return acc + val; });
};
```

---

## 6. 缓存系统设计

### 6.1 策略模式

```cpp
namespace ess::orm::cache {

// 缓存策略 concept
template <typename T>
concept cache_strategy = requires(typename T::key_type key) {
  { T::get(key) } -> std::same_as<typename T::value_type>;
  { T::set(key, std::declval<typename T::value_type>()) } -> std::same_as<void>;
  { T::invalidate(key) } -> std::same_as<void>;
  { T::clear() } -> std::same_as<void>;
};

// 静态 LRU 缓存（编译期大小）
template <typename Key, typename Value, std::size_t Size>
struct StaticLRU {
  using key_type = Key;
  using value_type = Value;

  static Value get(Key key);
  static void set(Key key, Value value);
  static void invalidate(Key key);
  static void clear();
};

// 动态 LRU 缓存（运行时大小）
template <typename Key, typename Value>
struct DynamicLRU {
  using key_type = Key;
  using value_type = Value;

  DynamicLRU(std::size_t capacity);

  Value get(Key key) const;
  void set(Key key, Value value);
  void invalidate(Key key);
  void clear();
};

// 用户自定义策略（注入）
template <typename Key, typename Value>
struct UserCustomCache {
  using key_type = Key;
  using value_type = Value;

  static Value get(Key key) {
    return YourCache::get(key);  // 用户实现
  }

  static void set(Key key, Value value) {
    YourCache::set(key, value);
  }
};

} // ess::orm::cache
```

### 6.2 缓存配置 Trait

```cpp
template <typename DBConfig>
struct CacheConfigTrait {
  using key_type = std::string;  // SQL + params hash
  using value_type = Row;

  // 策略选择
  using strategy = [] {
    if constexpr (config::detail::has_cache_strategy<DBConfig>) {
      return typename DBConfig::cache_strategy;
    } else {
      return DynamicLRU<key_type, value_type>;  // 默认
    }
  }();

  static constexpr std::size_t default_capacity = [] {
    if constexpr (config::detail::has_cache_capacity<DBConfig>) {
      return DBConfig::cache_capacity;
    } else {
      return 1000;  // 默认
    }
  }();
};
```

### 6.3 查询级缓存控制

```cpp
// 查询时指定缓存
auto result = query<Order, "SELECT * FROM orders WHERE id = ?", cache::enabled>(id);
auto result = query<Order, "SELECT ...", cache::disabled>();

// 或者通过 Attribute
// Field<"id", &Order::id, CacheDisabled>  // 不缓存该字段

// 表级缓存配置
// struct Order {
//   static constexpr bool enable_cache = true;
//   static constexpr std::size_t cache_ttl = 60;  // 秒
//   ...
// };
```

---

## 7. SQL 语法检测

### 7.1 递归下降解析器 + FixedString

```cpp
namespace ess::orm::parser {

// 解析结果
enum class ParseResult {
  Success,
  InvalidSyntax,
  UnsupportedFeature,
  TypeMismatch,
  // ...
};

// 诊断信息
struct Diagnostic {
  std::string message;
  std::size_t position;
  ParseResult severity;

  // 支持自定义格式化
  template <typename Formatter>
  std::string format() const {
    return Formatter::format(*this);
  }
};

// 默认错误提示（严肃）
struct DefaultFormatter {
  static std::string format(const Diagnostic& diag) {
    return fmt::format("Error at {}: {}", diag.position, diag.message);
  }
};

// 可爱的错误提示（用户自定义）
struct CuteFormatter {
  static std::string format(const Diagnostic& diag) {
    const char* emoji = [](ParseResult r) {
      switch (r) {
        case ParseResult::InvalidSyntax: return "😵";
        case ParseResult::TypeMismatch: return "😤";
        case ParseResult::UnsupportedFeature: return "🤔";
        default: return "💀";
      }
    }(diag.severity);
    return fmt::format("{} Oops! At position {}: {}", emoji, diag.position, diag.message);
  }
};

// SQL 解析器模板
template <meta::FixedString SQL, typename Formatter = DefaultFormatter>
class SQLParser {
public:
  static constexpr std::array<Diagnostic, 16> validate() {
    // 递归下降解析
    std::array<Diagnostic, 16> diagnostics{};
    std::size_t diag_count = 0;

    // 解析过程...

    return diagnostics;
  }

  static constexpr bool is_valid() {
    auto diagnostics = validate();
    for (const auto& diag : diagnostics) {
      if (diag.severity != ParseResult::Success) {
        return false;
      }
    }
    return true;
  }
};

} // ess::orm::parser
```

### 7.2 配置中的错误提示格式选择

```cpp
// ess_orm_user_config.hpp
struct UserConfig {
  using error_formatter = ess::orm::parser::CuteFormatter;  // 可爱的提示
  // 或 using error_formatter = ess::orm::parser::DefaultFormatter;
};

// config/config.hpp
template <typename UserCfg, typename DefaultCfg = config::DefaultConfig>
struct ConfigTrait {
  using error_formatter = [] {
    if constexpr (detail::has_error_formatter<UserCfg>) {
      return typename UserCfg::error_formatter;
    } else {
      return parser::DefaultFormatter;
    }
  }();
};
```

---

## 8. 用户接入与编译配置

### 8.1 CMake 配置模板

```cmake
# 用户项目的 CMakeLists.txt
include(FetchContent)

FetchContent_Declare(
  ess_orm
  GIT_REPOSITORY https://github.com/yourusername/ess_orm.git
  GIT_TAG main
)

# 配置选项
set(ESS_ORM_DIALECT "sqlite3" CACHE STRING "Database dialect (sqlite3, mysql, postgres)")
set(ESS_ORM_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/configs" CACHE STRING "User config directory")

# 根据配置设置编译选项
if(ESS_ORM_DIALECT STREQUAL "sqlite3")
  target_compile_definitions(ess_orm INTERFACE ESS_ORM_USE_SQLITE3)
elseif(ESS_ORM_DIALECT STREQUAL "mysql")
  target_compile_definitions(ess_orm INTERFACE ESS_ORM_USE_MYSQL)
  # 链接 MySQL 库
elseif(ESS_ORM_DIALECT STREQUAL "postgres")
  target_compile_definitions(ess_orm INTERFACE ESS_ORM_USE_POSTGRES)
  # 链接 PostgreSQL 库
endif()

# 用户配置路径
target_include_directories(ess_orm INTERFACE ${ESS_ORM_CONFIG_DIR})
```

### 8.2 库的自动检测

```cpp
// include/ess/orm/config/config.hpp
// 根据 CMake 定义的宏选择方言
#if defined(ESS_ORM_USE_SQLITE3)
  using default_dialect = dialect::Sqlite3;
#elif defined(ESS_ORM_USE_MYSQL)
  using default_dialect = dialect::MySQL;
#elif defined(ESS_ORM_USE_POSTGRES)
  using default_dialect = dialect::Postgres;
#else
  // 尝试从用户配置中获取
  using default_dialect = config::dialect;
#endif
```

---

## 9. 额外便利功能

### 9.1 序列化集成

```cpp
namespace ess::orm::serialize {

enum class Format {
  JSON,
  YAML,
  CSV,
  XML,
  MsgPack,
  Custom
};

// QueryResult 扩展
template <typename DBConfig, typename Table, meta::FixedString SQL, typename... Args>
std::string to_json(QueryResult<DBConfig, Table, SQL, Args...>&& result) {
  std::string out;
  out += "[";
  bool first = true;
  result.for_each([&](const Table& item) {
    if (!first) out += ",";
    first = false;
    Serializer<Table>::to_json(out, item);
  });
  out += "]";
  return out;
}

} // ess::orm::serialize
```

### 9.2 自动迁移工具

```bash
# 检查 Schema 变化并自动生成迁移 SQL
ess_orm_migrate --config configs/ess_orm_user_config.hpp --output migrations/
```

### 9.3 SQL 日志美化

```cpp
// config/ess_orm_user_config.hpp
struct UserConfig {
  static constexpr bool enable_sql_logging = true;
  using log_formatter = ColorizedLogFormatter;  // 彩色日志
  // 或 using log_formatter = JsonLogFormatter;  // JSON 格式（方便解析）
};

// 日志输出示例
// [2024-01-15 10:30:45] 🟢 SELECT * FROM orders WHERE id = ?  [1]  -- 5.2ms
```

### 9.4 性能分析集成

```cpp
// 自动收集慢查询
struct UserConfig {
  static constexpr std::chrono::milliseconds slow_query_threshold{100ms};  // 超过 100ms 记录
  static constexpr bool enable_performance_profiling = true;
};

// 运行时报告
ess_orm_report_performance();
// 输出：
// Slow Queries (3):
//   1. SELECT * FROM orders WHERE status = ?  [150ms]
//   2. SELECT * FROM products JOIN ...  [250ms]
//   3. INSERT INTO analytics ...  [120ms]
```

### 9.5 类型安全的查询构建器（可选）

```cpp
// 对于不想写 SQL 的用户
auto orders = query<Order>()
  .where(Order::status == 1 && Order::amount > 100)
  .order_by(Order::created_at, Descending)
  .limit(10)
  .execute();

// 编译期生成 SQL: "SELECT * FROM orders WHERE status = ? AND amount > ? ORDER BY created_at DESC LIMIT 10"
```

### 9.6 批量操作优化

```cpp
// 批量插入，单条 SQL
std::vector<Order> orders = {/* ... */};
bulk_insert<Order>(orders);

// 批量更新
bulk_update<Order>(orders, [](auto& o) { o.status = 1; });
```

---

## 10. 接口设计总结（"优雅"的关键）

### 统一的 fluent API

```cpp
// 核心原则：链式调用 + 隐式上下文

// 1. 单一数据库，最简单
transaction([] {
  query<Order, "INSERT INTO orders (user_id, amount) VALUES (?, ?)">(1, 100.0);
});

// 2. 多数据库，指定数据库
transaction<MainDB>([] {
  query<MainDB, Order, "INSERT ...">();
});

transaction<ReplicaDB>([] {
  query<ReplicaDB, Log, "INSERT ...">();
});

// 3. 结果处理
auto total = query<Order, "SELECT SUM(amount) FROM orders WHERE user_id = ?">(1)
  .first()  // 获取单行
  .as<long long>();

auto orders = query<Order, "SELECT * FROM orders WHERE status = ?">(1)
  .filter([](auto& o) { return o.amount > 100; })  // 过滤
  .map([](auto& o) { return OrderDTO{o.id, o.amount}; })  // 转换
  .to_json();  // 序列化

// 4. 缓存控制
auto result = query<Order, "SELECT * FROM orders WHERE id = ?",
                   cache::ttl(60)>(id);  // 缓存 60 秒

// 5. 自定义错误提示
query<Order, "SELECT * FROM unknown_table">();
// 编译错误："😵 Oops! At position 14: Table 'unknown_table' not found"
```

---

## 11. 各模块关键设计原则

| 模块         | 关键设计原则            |
| ------------ | ----------------------- |
| **配置**     | 编译期覆盖 + 类型安全   |
| **Context**  | 自动注册 + 懒初始化     |
| **事务**     | 隐式上下文 + 函数式风格 |
| **查询**     | 链式调用 + 函数式处理   |
| **序列化**   | 统一接口 + 用户可扩展   |
| **缓存**     | 策略注入 + 编译期选择   |
| **语法检测** | 递归下降 + 可自定义提示 |
| **用户接入** | CMake 模板 + 自动检测   |

---

## 参考资源

### API 文档

- [SQLite Performance Tips](https://www.sqlite.org/cvstrac/wiki?p=PerformanceGuidelines)
- [MySQL C API Reference](https://dev.mysql.com/doc/c-api/en/)
- [PostgreSQL libpq Reference](https://www.postgresql.org/docs/current/libpq.html)

### 相关项目

- [cpp-orm](https://github.com/yhirose/cpp-orm) - 轻量级 C++ ORM 库
- [Drogon](https://github.com/an-tao/drogon) - 高性能 C++ Web 框架，内置 ORM
- [sqlite_modern_cpp](https://github.com/aminroosta/sqlite_modern_cpp) - SQLite3 C++ 封装
- [dorm](https://github.com/wc-duck/dorm) - 支持 SQLite3 和 MySQL 的轻量级 ORM

---

## 设计理念总结

1. **类型安全**：充分利用 C++20 的编译期类型检查
2. **零开销抽象**：模板元编程避免运行时开销
3. **声明式 API**：用户通过定义 Schema 来描述数据结构
4. **函数式风格**：链式调用、隐式上下文
5. **可扩展性**：用户可以注入自定义策略
6. **优雅与便利**：减少样板代码，提升开发体验

这个文档整合了多数据库支持、事务处理、查询结果映射、缓存系统、语法检测等多个方面的设计建议，旨在实现一个既高性能又易用的现代 C++ ORM 库。
