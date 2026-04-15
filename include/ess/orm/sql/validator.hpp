#pragma once
#include <ess/orm/common/error.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parse_result.hpp>
#include <ess/orm/sql/parser.hpp>

namespace ess::orm::sql {

/*
 * SQL字面量运算符，返回解析结果对象
 * 进行基本语法验证
 */
template <meta::FixedString SQL>
consteval auto operator""_sql() -> SqlParseResult<SQL> {
  // 基本语法验证
  static_assert(!SqlParseResult<SQL>::lex_result.has_error,
                "SQL Lexer Error: Invalid SQL syntax");

  static_assert(!SqlParseResult<SQL>::parse_result.has_error,
                "SQL Parser Error: Invalid SQL structure");

  return SqlParseResult<SQL>{};
}

} // namespace ess::orm::sql
