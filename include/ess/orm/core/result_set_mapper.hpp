#pragma once
#include <ess/orm/core/row.hpp>
#include <ess/orm/core/statement.hpp>
#include <ess/orm/dsl/schema.hpp>

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

// 优化映射表的建立：编译期字段名排序+函数指针数组
template <concepts::table_type Table> struct SchemaMapper {
private:
  using schema = Table::Schema;
  using field_tuple = schema::fields;
  static constexpr std::size_t field_count = std::tuple_size_v<field_tuple>;

  // 编译期获取字段名
  template <std::size_t I> static constexpr std::string_view get_field_name() {
    using field_type = std::tuple_element_t<I, field_tuple>;
    return std::string_view(field_type::column_name);
  }

  // 编译期构建字段名数组并排序
  struct FieldInfo {
    std::string_view name;
    int index;

    constexpr bool operator<(const FieldInfo &other) const {
      return name < other.name;
    }
  };

  static constexpr auto build_sorted_field_infos() {
    std::array<FieldInfo, field_count> infos{};

    // 填充初始数据
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      ((infos[I] = FieldInfo{.name = get_field_name<I>(),
                             .index = static_cast<int>(I)}),
       ...);
    }(std::make_index_sequence<field_count>{});

    // 编译期冒泡排序（简单实现）
    for (std::size_t i = 0; i < field_count; ++i) {
      for (std::size_t j = 0; j < field_count - i - 1; ++j) {
        if (infos[j + 1] < infos[j]) {
          auto temp = infos[j];
          infos[j] = infos[j + 1];
          infos[j + 1] = temp;
        }
      }
    }

    return infos;
  }

  static constexpr auto sorted_field_infos = build_sorted_field_infos();

public:
  // 二分查找字段索引
  constexpr static int find_field_index(std::string_view field_name) {
    int left = 0;
    int right = static_cast<int>(field_count) - 1;

    while (left <= right) {
      int mid = left + (right - left) / 2;
      const auto &info = sorted_field_infos[mid];

      if (info.name == field_name) {
        return info.index;
      } else if (info.name < field_name) {
        left = mid + 1;
      } else {
        right = mid - 1;
      }
    }

    return -1; // 未找到
  }

  template <std::size_t FieldIdx>
  static void fill_field(Table &obj, core::Statement &stmt, int col_idx) {
    using cur_field = std::tuple_element_t<FieldIdx, field_tuple>;
    using member_type = typename cur_field::member_type;
    (obj.*(cur_field::pointer)) = get_column<member_type>(stmt, col_idx);
  }

  // 函数指针类型
  using FillFunc = void (*)(Table &, core::Statement &, int);

  // 编译期构建函数指针数组
  static constexpr auto build_fill_funcs() {
    std::array<FillFunc, field_count> funcs{};

    [&]<std::size_t... I>(std::index_sequence<I...>) {
      ((funcs[I] = [](Table &obj, core::Statement &stmt,
                      int col_idx) { fill_field<I>(obj, stmt, col_idx); }),
       ...);
    }(std::make_index_sequence<field_count>{});

    return funcs;
  }

  static constexpr auto fill_funcs = build_fill_funcs();
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
      // 使用二分查找优化字段索引查找
      for (int i = 0; i < size; ++i) {
        std::string name = stmt.column_name(i); // 保持字符串生命周期
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
        // MARK: 无法映射到表字段的列
        // 根据SQLite测试，以下情况会导致列名无法映射：
        // 1. 列别名（SELECT id AS user_id）→ 列名: "user_id" (不是"id")
        // 2. 表达式列（SELECT id + 1）→ 列名: "id + 1"
        // 3. 函数调用（SELECT UPPER(title)）→ 列名: "UPPER(title)"
        // 4. 聚合函数（SELECT COUNT(*)）→ 列名: "COUNT(*)"
        // 5. 字面量（SELECT 123）→ 列名: "123"
        //
        // 但以下复杂查询能正确映射：
        // - JOIN查询（表前缀被去除）
        // - 子查询（返回原始列名）
        // - UNION查询（使用第一个SELECT的列名）
        // - WITH子句（CTE）
        // - 表别名（被去除）
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
    // 直接使用函数指针数组调用，消除分支检查
    if (field_idx >= 0 && static_cast<std::size_t>(field_idx) <
                              schema_mapper::fill_funcs.size()) {
      schema_mapper::fill_funcs[field_idx](obj, stmt, col_idx);
    }
  }
};

} // namespace ess::orm
