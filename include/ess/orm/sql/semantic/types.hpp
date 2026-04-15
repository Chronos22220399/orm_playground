#pragma once
#include <cstdint>
#include <string_view>

namespace ess::orm::sql::semantic {

// SQL语义类型（比语法类型更细粒度）
enum class SqlType : uint8_t {
  Unknown,  // 未知类型
  Null,     // NULL类型
  Integer,  // 整数类型
  Floating, // 浮点类型
  Text,     // 文本类型
  Boolean,  // 布尔类型
  Any,      // 任意类型（用于占位符推断）

  // 复合类型（用于表达式结果）
  Numeric,    // 数值类型（Integer或Floating）
  Comparable, // 可比较类型
};

// 类型兼容性检查
constexpr bool is_type_compatible(SqlType expected, SqlType actual) {
  if (actual == SqlType::Any || expected == SqlType::Any) {
    return true; // 任意类型兼容
  }

  if (expected == actual) {
    return true; // 相同类型兼容
  }

  // 类型转换规则
  switch (expected) {
  case SqlType::Integer:
    return actual == SqlType::Floating; // 整数可以转为浮点数

  case SqlType::Floating:
    return actual == SqlType::Integer; // 浮点数可以接受整数

  case SqlType::Numeric:
    return actual == SqlType::Integer || actual == SqlType::Floating;

  case SqlType::Comparable:
    return actual == SqlType::Integer || actual == SqlType::Floating ||
           actual == SqlType::Text || actual == SqlType::Boolean;

  default:
    return false;
  }
}

// 类型提升（二元运算结果类型）
constexpr SqlType promote_type(SqlType lhs, SqlType rhs) {
  if (lhs == SqlType::Floating || rhs == SqlType::Floating) {
    return SqlType::Floating;
  }
  if (lhs == SqlType::Integer && rhs == SqlType::Integer) {
    return SqlType::Integer;
  }
  if (lhs == SqlType::Text || rhs == SqlType::Text) {
    return SqlType::Text; // 字符串连接
  }
  return SqlType::Unknown;
}

// 从字面量推断类型
constexpr SqlType infer_type_from_literal(std::string_view literal) {
  // 简单实现，实际需要更复杂的解析
  if (literal.empty())
    return SqlType::Unknown;

  // 检查是否为数字
  bool has_dot = false;
  bool is_number = true;

  for (char c : literal) {
    if (c == '.') {
      if (has_dot) {
        is_number = false;
        break;
      }
      has_dot = true;
    } else if (c < '0' || c > '9') {
      is_number = false;
      break;
    }
  }

  if (is_number) {
    return has_dot ? SqlType::Floating : SqlType::Integer;
  }

  // 检查是否为布尔值
  if (literal == "TRUE" || literal == "FALSE" || literal == "true" ||
      literal == "false") {
    return SqlType::Boolean;
  }

  // 检查是否为NULL
  if (literal == "NULL" || literal == "null") {
    return SqlType::Null;
  }

  // 默认为文本（可能是带引号的字符串）
  return SqlType::Text;
}

} // namespace ess::orm::sql::semantic