#pragma once
#include <ess/orm/core/row.hpp>
#include <ess/orm/core/statement.hpp>
#include <ess/orm/dsl/dsl.hpp>

namespace ess::orm {

template <typename T> inline T get_column(core::Statement &stmt, int index) {
  if constexpr (std::is_same_v<T, std::string>) {
    return stmt.column_text(index);
  } else if constexpr (std::is_floating_point_v<T>) {
    return stmt.column_double(index);
  } else if constexpr (std::is_integral_v<T>) {
    return stmt.column_int64(index);
  } else if constexpr (std::is_enum_v<T>) {
    return static_cast<T>(stmt.column_int(index));
  } else {
    // 其他类型处理
  }
}

// TODO: 优化映射表的建立或是采用更高效的方式
template <concepts::table_type Table> struct SchemaMapper {
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
  static void fill_field(Table &obj, core::Statement &stmt, int col_idx) {
    using cur_field = std::tuple_element_t<FieldIdx, field_tuple>;
    using member_type = typename cur_field::member_type;
    (obj.*(cur_field::pointer)) = get_column<member_type>(stmt, col_idx);
  }
};

template <typename T = void> struct ResultSetMapper {
  static Row map_row(core::Statement &stmt) {
    Row row;
    int size = stmt.column_count();
    for (int idx = 0; idx < size; ++idx) {
      std::string name = stmt.column_name(idx);
      meta::ColumnType type = stmt.column_type(idx);
      switch (type) {
      case meta::ColumnType::Int64: {
        row.add_column(name, DBValue{get_column<int64_t>(stmt, idx)});
        break;
      }
      case meta::ColumnType::Float: {
        row.add_column(name, DBValue{(get_column<double>(stmt, idx))});
        break;
      }
      case meta::ColumnType::Text: {
        row.add_column(name, DBValue{(get_column<std::string>(stmt, idx))});
        break;
      }
      case meta::ColumnType::Null:
        break;
      default:
        break;
      }
    }
    return row;
  }
};

template <concepts::table_type Table> struct ResultSetMapper<Table> {
  std::vector<int> m_col_to_field_map{};
  bool is_inialized = false;
  using schema_mapper = SchemaMapper<Table>;

  void init_mapper(core::Statement &stmt) {
    if (!is_inialized) {
      int size = stmt.column_count();
      m_col_to_field_map.resize(size);
      // 将 table 的字段下标写入 map
      for (int i = 0; i < size; ++i) {
        std::string name = stmt.column_name(i);
        int idx = schema_mapper::find_field_index(name);
        m_col_to_field_map[i] = idx;
      }
      is_inialized = true;
    }
  }

  Table map_row(core::Statement &stmt) {
    Table obj;
    for (int col_idx = 0; col_idx < m_col_to_field_map.size(); ++col_idx) {
      int field_idx = m_col_to_field_map[col_idx];
      if (field_idx == -1) {
        std::string error_info = "unknown column: ";
        error_info += stmt.column_name(col_idx);
        throw std::runtime_error(error_info);
      }
      // 分派到对应值上
      dispatch_fill(stmt, obj, field_idx, col_idx);
    }
    return obj;
  }

private:
  void dispatch_fill(core::Statement &stmt, Table &obj, int field_idx,
                     int col_idx) {
    auto fields = Table::Schema::make_fields();
    constexpr int N = std::tuple_size_v<decltype(fields)>;

    [&]<std::size_t... I>(std::index_sequence<I...>) {
      (([&]() {
         if (I == static_cast<std::size_t>(field_idx)) {
           schema_mapper::template fill_field<I>(obj, stmt, col_idx);
         }
       }()),
       ...);
    }(std::make_index_sequence<N>{});
  }
};

} // namespace ess::orm
