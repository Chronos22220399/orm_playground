#pragma once
#include <ess/orm/sql/common.hpp>

namespace ess::orm::sql {
enum class SQLErrorKind : uint8_t {
  None,
  InvalidToken,
  UnknownBeginning,
  ExpectedStarOrColumns,
  ExpectedFrom,
  ExpectedTableAfterFrom,
  ExpectedIdentifier,
  ExpectedIdentifierInWhereClause,
  ExpectedOperator,
  ExpectedLiteralOrPlaceHolder,
  ExpectedRightParen,
  ExpectedLeftParenAfterIn,
  ExpectedLiteralInList,
  ExpectedStringLiteralAfterLike,
  ExpectedAndInBetweenClause,
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
    else if constexpr (R.error == SQLErrorKind::ExpectedLiteralOrPlaceHolder)
      throw "SQL Error: Expected an literal or placeholder in WHERE clause";
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
    bool first_vis = true;
    while (true) {
      if (peek().type == TokenType::Identifier) {
        if (first_vis) {
          result.column_start = peek().pos;
          first_vis = false;
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

  // Parse: IN (SELECT ...) or IN (val1, val2, ...)
  constexpr void parse_in_clause(ParseResult &result) {
    advance(); // Consume 'IN'

    if (peek().type != TokenType::Lparen) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedLeftParenAfterIn;
      return;
    }
    advance(); // Consume '('

    if (peek().type == TokenType::Select) {
      auto res = parse_select();
      if (res.has_error) {
        result.has_error = true;
        result.error = res.error;
        return;
      }
    } else {
      // Parse hardcoded literal list: (1, 2, 'abc')
      while (true) {
        if (peek().type == TokenType::Number ||
            peek().type == TokenType::String) {
          advance();
        } else {
          result.has_error = true;
          result.error = SQLErrorKind::ExpectedLiteralInList;
          return;
        }

        if (peek().type == TokenType::Comma) {
          advance();
        } else {
          break; // End of list
        }
      }
    }

    if (peek().type == TokenType::Rparen) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedRightParen;
    }
  }

  // Parse: LIKE 'pattern'
  constexpr void parse_like_clause(ParseResult &result) {
    advance(); // Consume 'LIKE'

    if (peek().type == TokenType::String) {
      // TODO: You could validate the regex/wildcard pattern content here.
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedStringLiteralAfterLike;
    }
  }

  // Parse: BETWEEN val1 AND val2
  constexpr void parse_between_clause(ParseResult &result) {
    advance(); // Consume 'BETWEEN'

    // Match lower bound
    if (peek().type == TokenType::String || peek().type == TokenType::Number ||
        peek().type == TokenType::PlaceHolder) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedLiteralOrPlaceHolder;
      return;
    }

    // Match the 'AND' keyword
    if (peek().type == TokenType::And) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedAndInBetweenClause;
      return;
    }

    // Match upper bound
    if (peek().type == TokenType::String || peek().type == TokenType::Number ||
        peek().type == TokenType::PlaceHolder) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedLiteralOrPlaceHolder;
    }
  }

  // Parse: = , != , < , > , <= , >=
  constexpr void parse_binary_operator_clause(ParseResult &result) {
    advance(); // Consume operator

    // The right-hand side can be a literal, a placeholder, or a subquery
    if (peek().type == TokenType::String || peek().type == TokenType::Number ||
        peek().type == TokenType::PlaceHolder) {
      advance();
    } else if (peek().type == TokenType::Lparen) {
      advance(); // Consume '('

      if (peek().type == TokenType::Select) {
        auto res = parse_select();
        if (res.has_error) {
          result.has_error = true;
          result.error = res.error;
          return;
        }
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedStarOrColumns;
        return;
      }

      if (peek().type == TokenType::Rparen) {
        advance();
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedRightParen;
      }
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedLiteralOrPlaceHolder;
    }
  }

  constexpr void parse_primary_expr(ParseResult &result) {
    // 1. Handle parentheses (Either precedence grouping or subqueries)
    if (peek().type == TokenType::Lparen) {
      advance(); // Consume '('

      // Case A: (SELECT ...) -> Standalone subquery in expression
      if (peek().type == TokenType::Select) {
        auto res = parse_select();
        if (res.has_error) {
          result.has_error = true;
          result.error = res.error;
          return;
        }

        if (peek().type == TokenType::Rparen) {
          advance();
        } else {
          result.has_error = true;
          result.error = SQLErrorKind::ExpectedRightParen;
        }
        return;
      }

      // Case B: (id > 10 AND ...) -> Normal precedence grouping
      parse_where_clause(result);
      if (result.has_error)
        return;

      if (peek().type == TokenType::Rparen) {
        advance();
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedRightParen;
      }
      return;
    }

    // 2. Match left-hand side operand (Must be a column identifier)
    if (peek().type != TokenType::Identifier) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedIdentifierInWhereClause;
      return;
    }
    advance(); // Consume Identifier

    // 3. Dispatch to specific clause parsers based on the current lookahead
    // token
    switch (peek().type) {
    case TokenType::In:
      parse_in_clause(result);
      break;

    case TokenType::Like:
      parse_like_clause(result);
      break;

    case TokenType::Between:
      parse_between_clause(result);
      break;

    default:
      // If it's none of the keywords above, it must be a normal comparison
      // operator
      if (is_operator(peek().type)) {
        parse_binary_operator_clause(result);
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedOperator;
      }
      break;
    }
  }

  constexpr void parse_and_expr(ParseResult &result) {
    parse_primary_expr(result);
    if (result.has_error)
      return;

    while (peek().type == TokenType::And) {
      advance();
      parse_primary_expr(result);
      if (result.has_error)
        return;
    }
  }

  constexpr void parse_where_clause(ParseResult &result) {
    parse_and_expr(result);
    if (result.has_error)
      return;

    // parse or
    while (peek().type == TokenType::Or) {
      advance();
      parse_and_expr(result);
      if (result.has_error)
        return;
    }
  }

  constexpr ParseResult parse_select() {
    ParseResult result{};
    if (peek().type == TokenType::Select) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::UnknownBeginning;
      return result;
    }
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
    } else if (at_end() || peek().type == TokenType::Rparen) {
      return result;
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::InvalidEnd;
      return result;
    }
    return result;
  }

  [[nodiscard]] constexpr ParseResult parse() {
    if (peek().type == TokenType::Select) {
      return parse_select();
    }
    return ParseResult::make_error();
  }
};

} // namespace ess::orm::sql
