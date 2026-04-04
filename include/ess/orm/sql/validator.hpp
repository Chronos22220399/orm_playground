#pragma once
#include <ess/orm/common/error.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parser.hpp>

namespace ess::orm::sql {

/*
 * the tool check whether sql string has error
 */
template <meta::FixedString SQL> consteval auto operator""_sql() {
  using namespace ess::orm::sql;
  constexpr LexResult<128> lex_res = Lexer(SQL).template tokenize<128>();

  if (lex_res.has_error) {
    throw "SQL Lexer Error: Invalid SQL syntax detected!";
  }

  constexpr ParseResult parse_res = sql::Parser(lex_res.tokens).parse();
  if (parse_res.has_error) {
    throw "SQL Parser Error: Invalid SQL samentic detected!";
  }

  return SQL;
}

} // namespace ess::orm::sql
