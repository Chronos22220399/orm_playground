#pragma once
#include <algorithm>
#include <ess/orm/core/row.hpp>
#include <ess/orm/core/statement.hpp>
#include <ess/orm/dsl/schema.hpp>
#include <ess/orm/dsl/visitor.hpp>

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
  }
}

template <concepts::table_type Table> struct SchemaMapper {
private:
  using schema = Table::Schema;
  using field_tuple = schema::fields;
  static constexpr std::size_t field_count = std::tuple_size_v<field_tuple>;

  template <std::size_t I> static constexpr std::string_view get_field_name() {
    using field_type = std::tuple_element_t<I, field_tuple>;
    return std::string_view(field_type::column_name);
  }

  static constexpr size_t hash_str(std::string_view s) {
    size_t h = 5381;
    for (unsigned char c : s)
      h = ((h << 5) + h) + c;
    return h;
  }

  struct FieldInfo {
    std::string_view name;
    size_t hash;
    int index;
  };

  static constexpr auto build_field_infos() {
    std::array<FieldInfo, field_count> infos{};
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      ((infos[I] = FieldInfo{.name = get_field_name<I>(),
                             .hash = hash_str(get_field_name<I>()),
                             .index = static_cast<int>(I)}),
       ...);
    }(std::make_index_sequence<field_count>{});
    return infos;
  }

  static constexpr auto field_infos = build_field_infos();

public:
  static constexpr std::size_t field_count_v = field_count;

  static int find_field_index(std::string_view name) noexcept {
    if (name.empty())
      return -1;
    size_t h = hash_str(name);
    for (const auto &info : field_infos) {
      if (info.hash == h && info.name == name) {
        return info.index;
      }
    }
    return -1;
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
  using schema_mapper = SchemaMapper<Table>;
  static constexpr std::size_t field_count = schema_mapper::field_count_v;

  int m_field_to_col_map[field_count]{};
  bool is_inialized = false;

  void init_mapper(core::Statement &stmt) {
    if (!is_inialized) {
      std::fill_n(m_field_to_col_map, field_count, -1);

      int size = stmt.column_count();
      for (int col_idx = 0; col_idx < size; ++col_idx) {
        std::string_view name = stmt.column_name_view(col_idx);
        int field_idx = schema_mapper::find_field_index(name);
        if (field_idx != -1) {
          m_field_to_col_map[field_idx] = col_idx;
        } else {
          throw std::runtime_error(std::string("unknown column: ") +
                                   std::string(name));
        }
      }
      is_inialized = true;
    }
  }

  Table map_row(core::Statement &stmt) {
    Table obj;
    dsl::Visitor<Table>::foreach ([&](auto info) {
      constexpr size_t FieldIdx = decltype(info)::index;
      int col_idx = m_field_to_col_map[FieldIdx];
      if (col_idx >= 0) {
        using member_type = typename decltype(info)::member_type;
        auto &value = dsl::FieldAccessor<Table>::template get<FieldIdx>(obj);
        value = get_column<member_type>(stmt, col_idx);
      }
    });
    return obj;
  }
};

} // namespace ess::orm
