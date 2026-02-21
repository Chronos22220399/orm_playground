#pragma once
#include <ess/orm/sql/ast/common.hpp>

namespace ess::orm::sql {
enum class SQLErrorKind : uint8_t {
  None,
  InvalidToken,
  ExpectedStarOrColumns,
  ExpectedFrom,
  ExpectedTableAfterFrom,
  ExpectedIdentifier,
  ExpectedIdentifierInWhereClause,
  ExpectedOperator,
  ExpectedLiteral,
  InvalidEnd,
  UnknownError,
};

template <typename T>
concept result_type = requires(T res) {
  { res.has_error } -> std::convertible_to<bool>;
};

template <result_type auto R> consteval void check() {
  if constexpr (R.has_error) {
    if constexpr (R.error == SQLErrorKind::InvalidToken)
      throw "SQL Error: Invalid token";
    else if constexpr (R.error == SQLErrorKind::ExpectedStarOrColumns)
      throw "SQL Error: Expected * or columns after SELECT";
    else if constexpr (R.error == SQLErrorKind::ExpectedFrom)
      throw "SQL Error: Expected FROM after column list";
    else if constexpr (R.error == SQLErrorKind::ExpectedTableAfterFrom)
      throw "SQL Error: Expected table name after FROM";
    else if constexpr (R.error == SQLErrorKind::ExpectedIdentifier)
      throw "SQL Error: Expected identifier";
    else if constexpr (R.error == SQLErrorKind::ExpectedIdentifierInWhereClause)
      throw "SQL Error: Expected identifier in WHERE clause";
    else if constexpr (R.error == SQLErrorKind::ExpectedOperator)
      throw "SQL Error: Expected an operator after identifier";
    else if constexpr (R.error == SQLErrorKind::ExpectedLiteral)
      throw "SQL Error: Expected an literal in WHERE clause";
    else if constexpr (R.error == SQLErrorKind::InvalidEnd)
      throw "SQL Error: Expected Where clause or end";
    else
      throw "SQL Error: Unknown syntax error";
  }
}

struct Column {
  std::size_t pos;
  std::size_t len;
};

struct ParseResult {
  bool has_error = false;
  SQLErrorKind error = SQLErrorKind::None;
  std::size_t err_idx = 0;
  bool is_star = false;
  std::size_t column_start = 0;
  std::size_t column_count = 0;
  std::array<Column, 32> column_names{};

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
  const std::array<Token, TokenCount> &m_tokens;
  std::size_t m_pos = 0;

public:
  constexpr Parser(const std::array<Token, TokenCount> &tokens)
      : m_tokens(tokens), m_pos(0) {}

  constexpr bool at_end() const {
    return m_pos >= TokenCount || m_tokens[m_pos].type == TokenType::End;
  }

  constexpr Token advance() {
    if (at_end())
      return make_end();
    return m_tokens[m_pos++];
  }

  constexpr Token peek() const {
    if (at_end())
      return make_end();
    return m_tokens[m_pos];
  }

  constexpr Token peek_next() const {
    if (m_pos + 1 >= TokenCount || m_tokens[m_pos + 1].type == TokenType::End)
      return make_end();
    return m_tokens[m_pos + 1];
  }

  constexpr bool match(TokenType type, ParseResult &result,
                       SQLErrorKind error) {
    if (peek().type == type) {
      advance();
      return true;
    }
    result.has_error = true;
    result.error = error;
    return false;
  }

  constexpr void parse_columns(ParseResult &result) {
    bool first_vis = false;
    while (true) {
      if (peek().type == TokenType::Identifier) {
        if (first_vis) {
          result.column_start = peek().pos;
          first_vis = true;
        }
        result.add_column(Column{.pos = peek().pos, .len = peek().len});
        advance();
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedStarOrColumns;
        return;
      }

      if (peek().type == TokenType::Comma) {
        advance();
      } else {
        break;
      }
    }
  }

  constexpr bool is_operator(TokenType type) {
    switch (type) {
    case TokenType::Eq:
    case TokenType::Ne:
    case TokenType::Lt:
    case TokenType::Gt:
    case TokenType::Le:
    case TokenType::Ge:
      return true;
    default:
      return false;
    }
  }

  constexpr void parse_where_clause(ParseResult &result) {
    if (peek().type != TokenType::Identifier) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedIdentifierInWhereClause;
      return;
    }

    advance();

    // match the operator
    if (is_operator(peek().type)) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedOperator;
      return;
    }

    // match the rhs: string or number
    if (peek().type == TokenType::String || peek().type == TokenType::Number) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedOperator;
      return;
    }
  }

  constexpr ParseResult parse_select() {
    ParseResult result{};
    // parse star or columns
    if (peek().type == TokenType::Star) {
      result.is_star = true;
      advance();
    } else if (peek().type == TokenType::Identifier) {
      result.is_star = false;
      parse_columns(result);
      if (result.has_error)
        return result;
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedStarOrColumns;
      return result;
    }

    // parse from
    if (!match(TokenType::From, result, SQLErrorKind::ExpectedFrom)) {
      return result;
    }

    // parse table name
    if (!match(TokenType::Identifier, result,
               SQLErrorKind::ExpectedTableAfterFrom)) {
      return result;
    }

    if (peek().type == TokenType::Where) {
      advance();
      parse_where_clause(result);
    } else if (at_end()) {
      return result;
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::InvalidEnd;
      return result;
    }
    return result;
  }

  [[nodiscard]] constexpr ParseResult parse() {
    if (m_tokens[m_pos].type == TokenType::Select) {
      m_pos++;
      return parse_select();
    }
    return ParseResult::make_error();
  }
};

} // namespace ess::orm::sql
