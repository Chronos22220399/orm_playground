#pragma once
#include <ess/orm/dsl/dsl.hpp>
#include <string_view>
#include <tuple>

namespace ess::orm::sql::semantic {

// 简化的表模式检查器
template <typename TableType> class SimpleSchemaChecker {
public:
  using Schema = typename TableType::Schema;
  using FieldTuple = typename Schema::fields;

  constexpr SimpleSchemaChecker() = default;

  // 检查列是否存在于表模式中
  template <meta::FixedString ColumnName>
  static constexpr bool column_exists() {
    bool found = false;
    std::apply(
        [&](auto... fields) {
          ((void)((strings_equal<decltype(fields)::column_name, ColumnName>())
                      ? (found = true, true)
                      : false),
           ...);
        },
        FieldTuple{});
    return found;
  }

  // 编译期字符串比较工具
  template <meta::FixedString Str1, meta::FixedString Str2>
  static constexpr bool strings_equal() {
    if constexpr (Str1.size() != Str2.size()) {
      return false;
    }

    for (std::size_t i = 0; i < Str1.size(); ++i) {
      if (Str1[i] != Str2[i]) {
        return false;
      }
    }
    return true;
  }

  // 检查SELECT * 是否有效（总是有效）
  static constexpr bool check_select_star() { return true; }

  // 检查特定的列名
  template <meta::FixedString... ColumnNames>
  static constexpr bool check_columns() {
    return (column_exists<ColumnNames>() && ...);
  }
};

} // namespace ess::orm::sql::semantic