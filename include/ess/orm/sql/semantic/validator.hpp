#pragma once
#include <ess/orm/sql/parser_decl.hpp>
#include <ess/orm/sql/semantic/semantic_info.hpp>

namespace ess::orm::sql::semantic {

// 基础语义验证器
class BaseSemanticValidator {
public:
  constexpr BaseSemanticValidator() = default;

  // 验证占位符计数
  // expected_count: 期望的占位符数量（从查询参数推断）
  // result: 解析结果，包含占位符信息
  constexpr bool validate_placeholder_count(std::size_t expected_count,
                                            const ParseResult &result) const {
    if (result.semantic_result.placeholder_count != expected_count) {
      return false;
    }
    return true;
  }

  // 验证语句完整性
  constexpr bool
  validate_statement_completeness(const ParseResult &result) const {
    // 检查是否有未闭合的括号等
    // 目前简单实现，总是返回true
    return result.semantic_result.is_complete;
  }

  // 执行基础语义验证
  // expected_placeholder_count: 期望的占位符数量
  // result: 解析结果（将被修改以包含语义错误）
  constexpr void validate(std::size_t expected_placeholder_count,
                          ParseResult &result) const {
    // 清除之前的语义错误
    result.semantic_result.clear_semantic_error();

    // 验证占位符计数
    if (!validate_placeholder_count(expected_placeholder_count, result)) {
      result.set_semantic_error(SQLErrorKind::PlaceholderCountMismatch,
                                result.semantic_result.placeholder_count > 0
                                    ? result.semantic_result.placeholders[0].pos
                                    : 0);
      return;
    }

    // 验证语句完整性
    if (!validate_statement_completeness(result)) {
      result.set_semantic_error(SQLErrorKind::IncompleteStatement, 0);
      return;
    }

    // 验证聚合函数与GROUP BY的兼容性
    if (result.has_aggregate &&
        !result.semantic_result.has_aggregate_without_group_by) {
      // 如果有聚合函数但没有GROUP BY，检查是否在非聚合上下文中使用了聚合
      // 这里需要更复杂的逻辑，目前简单处理
    }
  }
};

// 模式感知语义验证器（需要表类型信息）
template <typename TableType> class SchemaAwareSemanticValidator {
public:
  constexpr SchemaAwareSemanticValidator() = default;

  // 验证列存在性
  constexpr bool validate_column_existence(const ParseResult &result) const {
    // 这里需要访问表的模式信息
    // 目前是占位符实现
    return true;
  }

  // 验证类型兼容性
  constexpr bool validate_type_compatibility(const ParseResult &result) const {
    // 这里需要访问表的类型信息
    // 目前是占位符实现
    return true;
  }

  // 执行模式感知语义验证
  constexpr void validate(ParseResult &result) const {
    // 先执行基础验证
    BaseSemanticValidator base_validator;
    // 注意：这里需要知道期望的占位符数量，这应该从调用者传递

    // 验证列存在性
    if (!validate_column_existence(result)) {
      result.set_semantic_error(SQLErrorKind::ColumnNotFound, 0);
      return;
    }

    // 验证类型兼容性
    if (!validate_type_compatibility(result)) {
      result.set_semantic_error(SQLErrorKind::TypeMismatch, 0);
      return;
    }

    // 验证表名匹配
    // 这里需要检查查询中的表名是否与TableType匹配
  }
};

} // namespace ess::orm::sql::semantic