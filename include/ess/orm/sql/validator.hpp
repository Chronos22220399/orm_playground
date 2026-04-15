#pragma once
#include <ess/orm/common/error.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parser.hpp>
#include <ess/orm/sql/semantic/validator.hpp>

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
 * 基础版本：只进行语法和基础语义验证
 */
template <meta::FixedString SQL> consteval auto operator""_sql() {
  using namespace ess::orm::sql;
  constexpr LexResult<128> lex_res = Lexer(SQL).template tokenize<128>();

  static_assert(!lex_res.has_error,
                "SQL Lexer Error: Invalid SQL syntax detected!");

  constexpr ParseResult parse_res = Parser(lex_res.tokens).parse();
  static_assert(!parse_res.has_error,
                "SQL Parser Error: Invalid SQL syntax detected!");

  // 基础语义验证（占位符计数验证等）
  // 注意：基础版本不验证占位符数量，因为不知道期望的数量
  // 这将在运行时查询执行时验证

  return SQL;
}

/*
 * 带占位符数量验证的版本
 * 用于编译期验证占位符数量
 */
template <meta::FixedString SQL, std::size_t ExpectedPlaceholders = 0>
consteval auto validated_sql() {
  using namespace ess::orm::sql;
  constexpr LexResult<128> lex_res = Lexer(SQL).template tokenize<128>();

  static_assert(!lex_res.has_error,
                "SQL Lexer Error: Invalid SQL syntax detected!");

  constexpr ParseResult parse_res = Parser(lex_res.tokens).parse();
  static_assert(!parse_res.has_error,
                "SQL Parser Error: Invalid SQL syntax detected!");

  // 基础语义验证：占位符计数
  if constexpr (ExpectedPlaceholders != 0) {
    // 直接检查占位符数量
    if constexpr (parse_res.semantic_result.placeholder_count !=
                  ExpectedPlaceholders) {
      // 触发编译错误
      static_assert(parse_res.semantic_result.placeholder_count ==
                        ExpectedPlaceholders,
                    "SQL Semantic Error: Placeholder count mismatch!");
    }
  }

  return SQL;
}

} // namespace ess::orm::sql
