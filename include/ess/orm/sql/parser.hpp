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
  UnknownError,
};

struct ParseResult {
  bool has_error = false;
  SQLErrorKind error = SQLErrorKind::None;
  std::size_t err_idx = 0;
  bool is_star = false;
  std::size_t column_start = 0;
  std::size_t column_count = 0;

  static constexpr ParseResult
  make_error(SQLErrorKind e = SQLErrorKind::UnknownError) {
    return {.has_error = true, .error = e};
  }
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
    else
      throw "SQL Error: Unknown syntax error";
  }
}

template <std::size_t TokenCount> class Parser {
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

  constexpr void parse_columns() {}

  constexpr ParseResult parse_select() {
    ParseResult result{};
    if (m_tokens[m_pos].type == TokenType::Star) {
      result.is_star = true;
    } else if (m_tokens[m_pos].type == TokenType::Identifier) {
      result.is_star = false;
      parse_columns();
    }
    if (m_tokens[m_pos].type == TokenType::From) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedStarOrColumns;
      return result;
    }
    m_pos++;

    if (m_tokens[m_pos++].type != TokenType::From) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedFrom;
      return result;
    }

    if (m_tokens[m_pos++].type != TokenType::Identifier) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedTableAfterFrom;
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
