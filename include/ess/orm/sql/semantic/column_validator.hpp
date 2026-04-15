#pragma once
#include <ess/orm/common/meta.hpp>
#include <ess/orm/dsl/dsl.hpp>
#include <ess/orm/sql/parser_decl.hpp>
#include <ess/orm/sql/semantic/semantic_info.hpp>
#include <ess/orm/sql/sql_error_kind.hpp>
#include <string_view>
#include <tuple>

namespace ess::orm::sql::semantic {

// 列存在性验证器
template <typename TableType> class ColumnExistenceValidator {
public:
  constexpr ColumnExistenceValidator() = default;

  // 验证列是否存在
  template <meta::FixedString SQL>
  constexpr bool validate_column_existence(const ParseResult &result) const {
    if (result.has_error) {
      return false;
    }

    // 检查SELECT语句中的列
    if (!result.is_insert && !result.is_update && !result.is_delete) {
      return validate_select_columns<SQL>(result);
    }

    // 检查INSERT语句中的列
    if (result.is_insert) {
      return validate_insert_columns<SQL>(result);
    }

    // 检查UPDATE语句中的列
    if (result.is_update) {
      return validate_update_columns<SQL>(result);
    }

    // DELETE语句没有列需要验证（除了WHERE子句）
    return true;
  }

private:
  // 验证SELECT语句中的列
  template <meta::FixedString SQL>
  constexpr bool validate_select_columns(const ParseResult &result) const {
    if (result.is_star) {
      // SELECT * 总是有效的
      return true;
    }

    for (std::size_t i = 0; i < result.column_count; ++i) {
      const auto &col = result.column_names[i];
      std::string_view col_name = extract_column_name<SQL>(result, col);

      if (col_name.empty()) {
        return false;
      }

      if (!column_exists(col_name)) {
        return false;
      }
    }

    return true;
  }

  // 验证INSERT语句中的列
  template <meta::FixedString SQL>
  constexpr bool validate_insert_columns(const ParseResult &result) const {
    const auto &insert = result.insert_stmt;

    for (std::size_t i = 0; i < insert.column_count; ++i) {
      const auto &col = insert.columns[i];
      std::string_view col_name = extract_column_name<SQL>(result, col);

      if (col_name.empty()) {
        return false;
      }

      if (!column_exists(col_name)) {
        return false;
      }
    }

    return true;
  }

  // 验证UPDATE语句中的列
  template <meta::FixedString SQL>
  constexpr bool validate_update_columns(const ParseResult &result) const {
    const auto &update = result.update_stmt;

    for (std::size_t i = 0; i < update.set_count; ++i) {
      const auto &col = update.set_columns[i];
      std::string_view col_name = extract_column_name<SQL>(result, col);

      if (col_name.empty()) {
        return false;
      }

      if (!column_exists(col_name)) {
        return false;
      }
    }

    return true;
  }

  // 从SQL字符串中提取列名
  template <meta::FixedString SQL>
  static constexpr std::string_view
  extract_column_name(const ParseResult &result, const Column &col) {
    if (col.len == 0) {
      return "";
    }

    // 从SQL字符串中提取列名部分
    constexpr std::string_view sql_view = SQL;
    if (col.pos + col.len > sql_view.size()) {
      return "";
    }

    return sql_view.substr(col.pos, col.len);
  }

  // 检查列是否存在于表中
  constexpr bool column_exists(std::string_view col_name) const {
    if (col_name.empty()) {
      return false;
    }

    // 使用表的Schema信息检查列是否存在
    using Schema = typename TableType::Schema;
    using FieldTuple = typename Schema::fields;

    bool found = false;
    std::apply(
        [&](auto... fields) {
          ((void)((col_name == std::string_view(fields.column_name))
                      ? (found = true, true)
                      : false),
           ...);
        },
        FieldTuple{});

    return found;
  }
};

} // namespace ess::orm::sql::semantic