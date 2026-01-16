#pragma once
#include <ess/orm/config/config.hpp>
#include <ess/orm/dialect.hpp>
#include <ess/orm/dsl.hpp>
#include <ess/orm/row.hpp>
#include <fmt/format.h>

namespace ess::orm {

template <typename T> inline T get_column(sqlite3_stmt *stmt, int index) {
  if constexpr (std::is_same_v<T, std::string>) {
    return reinterpret_cast<const char *>(sqlite3_column_text(stmt, index));
  } else if constexpr (std::is_floating_point_v<T>) {
    return static_cast<T>(sqlite3_column_double(stmt, index));
  } else if constexpr (std::is_integral_v<T>) {
    return static_cast<T>(sqlite3_column_int(stmt, index));
  } else if constexpr (std::is_enum_v<T>) {
    return static_cast<T>(sqlite3_column_int(stmt, index));
  } else {
    // 其他类型处理
  }
}

inline std::string get_column_name(sqlite3_stmt *stmt, int index) {
  return reinterpret_cast<const char *>(sqlite3_column_name(stmt, index));
}

template <dsl::table_type Table> struct SchemaMapper {
private:
  using schema = Table::Schema;
  using field_tuple = schema::fields;

public:
  constexpr static int find_field_index(std::string_view field_name) {
    int found_idx = -1;
    std::apply(
        [&](auto... fields) {
          int current_idx = 0;
          ((void)((field_name == std::string_view(fields.column_name))
                      ? (found_idx = current_idx, true)
                      : (current_idx++, false)),
           ...);
        },
        field_tuple{});
    return found_idx;
  }

  template <std::size_t FieldIdx>
  constexpr static void fill_field(Table &obj, sqlite3_stmt *stmt,
                                   int col_idx) {
    using cur_field = std::tuple_element_t<FieldIdx, field_tuple>;
    using member_type = cur_field::member_type;
    obj.*(cur_field::pointer) = get_column<member_type>(stmt, col_idx);
  }
};

template <dsl::table_type Table> struct ResultSetMapper {
  std::vector<int> m_col_to_field_map{};
  bool is_inialized = false;
  using schema_mapper = SchemaMapper<Table>;

  void init_mapper(sqlite3_stmt *stmt) {
    if (!is_inialized) {
      int size = sqlite3_column_count(stmt);
      m_col_to_field_map.resize(size);
      // 将 table 的字段下标写入 map
      for (int i = 0; i < size; ++i) {
        std::string_view name = sqlite3_column_name(stmt, i);
        int idx = schema_mapper::find_field_index(name);
        m_col_to_field_map[i] = idx;
      }
      is_inialized = true;
    }
  }

  void map_row(sqlite3_stmt *stmt, Table &obj) {
    for (int col_idx = 0; col_idx < m_col_to_field_map.size(); ++col_idx) {
      int field_idx = m_col_to_field_map[col_idx];
      if (field_idx == -1) {
        std::string error_info = "unknown column: ";
        error_info += sqlite3_column_name(stmt, col_idx);
        throw std::runtime_error(error_info);
      }
      // 分派到对应值上
      dispatch_fill(stmt, obj, field_idx, col_idx);
    }
  }

  Row map_row(sqlite3_stmt *stmt) {
    Row row;
    int size = sqlite3_column_count(stmt);
    for (int idx = 0; idx < size; ++idx) {
      std::string name = get_column_name(stmt, idx);
      int type = sqlite3_column_type(stmt, idx);
      switch (type) {
      case SQLITE_INTEGER: {
        long long val = sqlite3_column_int64(stmt, idx);
        row.add_column(name, val);
        break;
      }
      case SQLITE_FLOAT: {
        row.add_column(name, DBValue{(get_column<double>(stmt, idx))});
        break;
      }
      case SQLITE_TEXT: {
        row.add_column(name, DBValue{(get_column<std::string>(stmt, idx))});
        break;
      }
      case SQLITE_NULL:
        break;
      default:
        break;
      }
    }
    return row;
  }

private:
  void dispatch_fill(sqlite3_stmt *stmt, Table &obj, int field_idx,
                     int col_idx) {
    auto fields = Table::Schema::make_fields();
    constexpr int N = std::tuple_size_v<decltype(fields)>;
    static_for<0, N>([&](auto I) {
      if (I == field_idx) {
        schema_mapper::template fill_field<I>(obj, stmt, col_idx);
      }
    });
  }

  template <std::size_t Beg, std::size_t End, typename Func>
  static void static_for(Func &&func) {
    if constexpr (Beg < End) {
      std::invoke(func, std::integral_constant<std::size_t, Beg>());
      static_for<Beg + 1, End>(func);
    }
  }
};

template <typename Table, meta::FixedString SQL> auto query(auto &&...args) {
  static_assert(dsl::table_type<Table>, "请使用持有 Schema 的 Table 类型");
  // 静态校验

  if (std::is_same_v<config::dialect, dialect::Postgres>) {
    fmt::println("Postgres");
  } else if (std::is_same_v<config::dialect, dialect::Sqlite>) {
    fmt::println("Sqlite");
  }
}

} // namespace ess::orm
