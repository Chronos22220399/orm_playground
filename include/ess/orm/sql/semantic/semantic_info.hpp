#pragma once
#include <array>
#include <cstdint>
#include <ess/orm/sql/semantic/types.hpp>
#include <ess/orm/sql/sql_error_kind.hpp>

namespace ess::orm::sql::semantic {

// 占位符信息
struct PlaceholderInfo {
  std::size_t pos = 0;                  // 在SQL字符串中的位置
  std::size_t len = 0;                  // 长度（通常为1）
  SqlType inferred_type = SqlType::Any; // 推断的类型
  bool is_bound = false;                // 是否已绑定类型
};

// 列语义信息
struct ColumnSemanticInfo {
  std::size_t pos = 0;                      // 在SQL字符串中的位置
  std::size_t len = 0;                      // 列名长度
  SqlType expected_type = SqlType::Unknown; // 期望的类型
  SqlType actual_type = SqlType::Unknown;   // 实际的类型（从表达式推断）
  bool is_valid = true;                     // 列是否有效
  std::size_t table_pos = 0;                // 表名位置（如果有）
  std::size_t table_len = 0;                // 表名长度
};

// 函数调用信息
struct FunctionCallInfo {
  std::size_t name_pos = 0;               // 函数名位置
  std::size_t name_len = 0;               // 函数名长度
  std::size_t arg_count = 0;              // 参数数量
  SqlType return_type = SqlType::Unknown; // 返回类型
  bool is_valid = true;                   // 函数调用是否有效
};

// 语义分析结果
struct SemanticResult {
  bool has_semantic_error = false;                  // 是否有语义错误
  SQLErrorKind semantic_error = SQLErrorKind::None; // 语义错误类型
  std::size_t semantic_err_idx = 0;                 // 语义错误位置

  // 占位符信息
  std::size_t placeholder_count = 0;              // 占位符总数
  std::array<PlaceholderInfo, 32> placeholders{}; // 占位符详细信息

  // 列语义信息
  std::size_t semantic_column_count = 0;                 // 需要语义分析的列数
  std::array<ColumnSemanticInfo, 32> semantic_columns{}; // 列语义信息

  // 函数调用信息
  std::size_t function_call_count = 0;               // 函数调用数量
  std::array<FunctionCallInfo, 16> function_calls{}; // 函数调用信息

  // 语句完整性检查
  bool is_complete = true;                     // 语句是否完整
  bool has_aggregate_without_group_by = false; // 是否有聚合但缺少GROUP BY

  // 添加占位符信息
  constexpr void add_placeholder(std::size_t pos, std::size_t len = 1) {
    if (placeholder_count < placeholders.size()) {
      placeholders[placeholder_count++] = {pos, len, SqlType::Any, false};
    }
  }

  // 添加列语义信息
  constexpr void add_column_semantic(std::size_t pos, std::size_t len,
                                     SqlType expected = SqlType::Unknown,
                                     SqlType actual = SqlType::Unknown) {
    if (semantic_column_count < semantic_columns.size()) {
      semantic_columns[semantic_column_count++] = {pos,  len, expected, actual,
                                                   true, 0,   0};
    }
  }

  // 添加函数调用信息
  constexpr void add_function_call(std::size_t name_pos, std::size_t name_len,
                                   std::size_t arg_count = 0,
                                   SqlType return_type = SqlType::Unknown) {
    if (function_call_count < function_calls.size()) {
      function_calls[function_call_count++] = {name_pos, name_len, arg_count,
                                               return_type, true};
    }
  }

  // 设置语义错误
  constexpr void set_semantic_error(SQLErrorKind error, std::size_t idx = 0) {
    has_semantic_error = true;
    semantic_error = error;
    semantic_err_idx = idx;
  }

  // 清除语义错误
  constexpr void clear_semantic_error() {
    has_semantic_error = false;
    semantic_error = SQLErrorKind::None;
    semantic_err_idx = 0;
  }
};

} // namespace ess::orm::sql::semantic