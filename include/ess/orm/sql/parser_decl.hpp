#pragma once
#include <array>
#include <ess/orm/sql/sql_error_kind.hpp>
#include <ess/orm/sql/token.hpp>

namespace ess::orm::sql {

struct Column {
  std::size_t pos;
  std::size_t len;
  std::size_t alias_pos = 0;
  std::size_t alias_len = 0;
  bool has_table_prefix = false;
  std::size_t table_pos = 0;
  std::size_t table_len = 0;
};

struct TableAlias {
  std::size_t pos = 0;
  std::size_t len = 0;
};

struct Table {
  std::size_t name_pos = 0;
  std::size_t name_len = 0;
  TableAlias alias{};
  bool is_subquery = false;
};

struct JoinInfo {
  std::size_t name_pos = 0;
  std::size_t name_len = 0;
  TableAlias alias{};
  bool is_subquery = false;
  TokenType join_type = TokenType::End;
  std::size_t on_pos = 0;
  std::size_t on_len = 0;
};

struct ParseResult {
  bool has_error = false;
  SQLErrorKind error = SQLErrorKind::None;
  std::size_t err_idx = 0;
  bool is_star = false;
  bool has_aggregate = false;
  bool has_arithmetic = false;
  bool has_left_column = false;
  std::size_t column_start = 0;
  std::size_t column_count = 0;
  std::array<Column, 32> column_names{};
  Table table{};
  std::array<JoinInfo, 8> joins{};
  std::size_t join_count = 0;
  std::size_t limit_pos = 0;
  std::size_t limit_len = 0;
  std::size_t offset_pos = 0;
  std::size_t offset_len = 0;

  static constexpr ParseResult
  make_error(SQLErrorKind e = SQLErrorKind::UnknownError) {
    return {.has_error = true, .error = e};
  }

  constexpr void add_column(Column col) {
    if (column_count < column_names.size()) {
      column_names[column_count++] = col;
    }
  }
};

template <std::size_t TokenCount> class Parser {
public:
  constexpr Parser(const std::array<Token, TokenCount> &tokens)
      : m_tokens(tokens), m_pos(0) {}

  constexpr ParseResult parse_select(bool allow_rparen_end = false);

  constexpr ParseResult parse_compound_select(bool allow_rparen_end = false);

  constexpr void parse_with_clause(ParseResult &result);

  [[nodiscard]] constexpr ParseResult parse();

private:
  constexpr bool at_end() const;

  constexpr Token advance();

  constexpr Token peek() const;

  constexpr Token peek_next() const;

  constexpr Token previous() const;

  constexpr bool match(TokenType type, ParseResult &result, SQLErrorKind error);

  constexpr void parse_columns(ParseResult &result);

  constexpr void parse_select_item(ParseResult &result);

  constexpr bool is_operator(TokenType type);

  constexpr bool is_aggregate_func(TokenType type);

  constexpr bool is_arithmetic_operator(TokenType type);

  constexpr void parse_in_clause(ParseResult &result);

  constexpr void parse_like_clause(ParseResult &result);

  constexpr void parse_between_clause(ParseResult &result);

  constexpr void parse_not_expr(ParseResult &result);

  constexpr void parse_exists_clause(ParseResult &result);

  constexpr void parse_is_clause(ParseResult &result);

  constexpr void parse_aggregate_expr(ParseResult &result);

  constexpr void parse_group_by_item(ParseResult &result);

  constexpr void parse_group_by_clause(ParseResult &result);

  constexpr void parse_having_clause(ParseResult &result);

  constexpr void parse_order_by_item(ParseResult &result);

  constexpr void parse_order_by_clause(ParseResult &result);

  constexpr void parse_binary_operator_clause(ParseResult &result);

  constexpr void parse_arithmetic_expr(ParseResult &result);

  constexpr void parse_primary_expr(ParseResult &result);

  constexpr void parse_and_expr(ParseResult &result);

  constexpr void parse_condition_expr(ParseResult &result);

  constexpr void parse_where_clause(ParseResult &result);

private:
  const std::array<Token, TokenCount> &m_tokens;
  std::size_t m_pos = 0;
  bool m_in_subquery = false;
};

} // namespace ess::orm::sql