#pragma once
#include <ess/orm/dsl/dsl.hpp>
#include <ess/orm/sql/parser_decl.hpp>
#include <ess/orm/sql/semantic/semantic_info.hpp>
#include <ess/orm/sql/sql_error_kind.hpp>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace ess::orm::sql::semantic {

// 类型兼容性验证器
template <typename TableType> class TypeCompatibilityValidator {
public:
  constexpr TypeCompatibilityValidator() = default;

  // 验证类型兼容性
  constexpr bool validate_type_compatibility(const ParseResult &result) const {
    if (result.has_error) {
      return false;
    }

    // 检查SELECT语句中的类型兼容性
    if (!result.is_insert && !result.is_update && !result.is_delete) {
      return validate_select_types(result);
    }

    // 检查INSERT语句中的类型兼容性
    if (result.is_insert) {
      return validate_insert_types(result);
    }

    // 检查UPDATE语句中的类型兼容性
    if (result.is_update) {
      return validate_update_types(result);
    }

    // DELETE语句没有类型兼容性问题
    return true;
  }

private:
  // 验证SELECT语句中的类型兼容性
  constexpr bool validate_select_types(const ParseResult &result) const {
    // SELECT语句的类型兼容性验证比较复杂
    // 需要检查表达式中的类型是否兼容
    // 目前简单实现，总是返回true
    return true;
  }

  // 验证INSERT语句中的类型兼容性
  constexpr bool validate_insert_types(const ParseResult &result) const {
    const auto &insert = result.insert_stmt;

    // 检查INSERT VALUES中的类型
    // 这里需要检查VALUES子句中的表达式类型是否与列类型兼容
    // 目前简单实现，总是返回true
    return true;
  }

  // 验证UPDATE语句中的类型兼容性
  constexpr bool validate_update_types(const ParseResult &result) const {
    const auto &update = result.update_stmt;

    // 检查SET子句中的类型兼容性
    // 这里需要检查SET表达式类型是否与列类型兼容
    // 目前简单实现，总是返回true
    return true;
  }

  // 获取列的类型信息
  template <typename Field> constexpr auto get_column_type() const {
    using MemberType = typename Field::member_type;
    return std::type_identity<MemberType>{};
  }

  // 检查类型是否兼容
  template <typename ColumnType, typename ExprType>
  constexpr bool is_type_compatible() const {
    // 基本类型兼容性检查
    if constexpr (std::is_same_v<ColumnType, ExprType>) {
      return true;
    }

    // 数值类型兼容性
    if constexpr (std::is_arithmetic_v<ColumnType> &&
                  std::is_arithmetic_v<ExprType>) {
      return true;
    }

    // 字符串类型兼容性
    if constexpr (std::is_same_v<ColumnType, std::string> &&
                  (std::is_same_v<ExprType, std::string> ||
                   std::is_same_v<ExprType, const char *>)) {
      return true;
    }

    // 枚举类型兼容性
    if constexpr (std::is_enum_v<ColumnType> && std::is_integral_v<ExprType>) {
      return true;
    }

    return false;
  }
};

} // namespace ess::orm::sql::semantic