#pragma once
#include <ess/orm/common/error.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parser.hpp>

namespace ess::orm::sql {

namespace details {

template <bool Condition> constexpr bool validate_sql_impl() {
  if constexpr (Condition) {
    return true;
  } else {
    return sizeof(char);
  }
}

} // namespace details

/*
 * the tool check whether sql string has error
 */
template <meta::FixedString SQL> consteval auto operator""_sql() {
  using namespace ess::orm::sql;
  constexpr LexResult<128> lex_res = Lexer(SQL).template tokenize<128>();

  static_assert(!lex_res.has_error,
                "SQL Lexer Error: Invalid SQL syntax detected!");

  constexpr ParseResult parse_res = Parser(lex_res.tokens).parse();
  static_assert(!parse_res.has_error,
                "SQL Parser Error: Invalid SQL semantics detected!");

  return SQL;
}

} // namespace ess::orm::sql
