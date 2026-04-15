#pragma once
#include <ess/orm/dsl/dsl.hpp>
#include <ess/orm/sql/semantic/column_extractor.hpp>
#include <string_view>
#include <tuple>

namespace ess::orm::sql::semantic {

// 表模式检查器
template <typename TableType> class SchemaChecker {
public:
  using Schema = typename TableType::Schema;
  using FieldTuple = typename Schema::fields;

  constexpr SchemaChecker() = default;

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

  // 检查多个列是否都存在
  template <meta::FixedString... ColumnNames>
  static constexpr bool all_columns_exist() {
    return (column_exists<ColumnNames>() && ...);
  }

  // 从解析结果中检查SELECT语句的列
  template <meta::FixedString SQL>
  static constexpr bool check_select_columns(const ParseResult &result) {
    if (result.is_star) {
      // SELECT * 总是有效的
      return true;
    }

    // 编译期检查每个列
    return check_select_columns_impl<SQL, 0>(result);
  }

  // 编译期检查SELECT列数组的辅助函数
  template <meta::FixedString SQL, std::size_t Index>
  static constexpr bool check_select_columns_impl(const ParseResult &result) {
    if constexpr (Index >= 32) {
      return true;
    } else if constexpr (Index < result.column_count) {
      constexpr auto &col = result.column_names[Index];
      constexpr auto col_name =
          ColumnNameExtractor<SQL>::template extract_column_name<col.pos,
                                                                 col.len>();

      if (!column_exists<col_name>()) {
        return false;
      }

      return check_select_columns_impl<SQL, Index + 1>(result);
    } else {
      return true;
    }
  }

  // 从解析结果中检查INSERT语句的列
  template <meta::FixedString SQL>
  static constexpr bool check_insert_columns(const ParseResult &result) {
    const auto &insert = result.insert_stmt;

    // 编译期检查每个列
    return check_insert_columns_impl<SQL, 0>(result);
  }

  // 编译期检查INSERT列数组的辅助函数
  template <meta::FixedString SQL, std::size_t Index>
  static constexpr bool check_insert_columns_impl(const ParseResult &result) {
    if constexpr (Index >= 32) {
      return true;
    } else if constexpr (Index < result.insert_stmt.column_count) {
      constexpr auto &col = result.insert_stmt.columns[Index];
      constexpr auto col_name =
          ColumnNameExtractor<SQL>::template extract_column_name<col.pos,
                                                                 col.len>();

      if (!column_exists<col_name>()) {
        return false;
      }

      return check_insert_columns_impl<SQL, Index + 1>(result);
    } else {
      return true;
    }
  }

  // 从解析结果中检查UPDATE语句的列
  template <meta::FixedString SQL>
  static constexpr bool check_update_columns(const ParseResult &result) {
    const auto &update = result.update_stmt;

    // 编译期检查SET子句中的每个列
    return check_update_columns_impl<SQL, 0>(result);
  }

  // 编译期检查UPDATE列数组的辅助函数
  template <meta::FixedString SQL, std::size_t Index>
  static constexpr bool check_update_columns_impl(const ParseResult &result) {
    if constexpr (Index >= 32) {
      return true;
    } else if constexpr (Index < result.update_stmt.set_count) {
      constexpr auto &col = result.update_stmt.set_columns[Index];
      constexpr auto col_name =
          ColumnNameExtractor<SQL>::template extract_column_name<col.pos,
                                                                 col.len>();

      if (!column_exists<col_name>()) {
        return false;
      }

      return check_update_columns_impl<SQL, Index + 1>(result);
    } else {
      return true;
    }
  }

  // 综合检查：根据语句类型检查相应的列
  template <meta::FixedString SQL>
  static constexpr bool check_all_columns(const ParseResult &result) {
    if (result.has_error) {
      return false;
    }

    if (result.is_insert) {
      return check_insert_columns<SQL>(result);
    } else if (result.is_update) {
      return check_update_columns<SQL>(result);
    } else if (result.is_delete) {
      // DELETE语句没有列需要检查（除了WHERE子句，暂时不检查）
      return true;
    } else {
      // SELECT语句
      return check_select_columns<SQL>(result);
    }
  }
};

} // namespace ess::orm::sql::semantic