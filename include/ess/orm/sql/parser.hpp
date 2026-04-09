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
  ExpectedLiteralOrPlaceHolderInList,
  ExpectedStringLiteralAfterLike,
  ExpectedAndInBetweenClause,
  ExpectedNullAfterNotInIsClause,
  ExpectedNullAfterInInIsClause,
  ExpectedByAfterGroup,
  ExpectedByAfterGroupInGroupByClause,
  ExpectedIdentifierAfterByInGroupByClause,
  HavingWithoutGroupBy,
  ExpectedByAfterOrderInOrderByClause,
  ExpectedIdentifierAfterByInOrderByClause,
  ExpectedLeftParenAfterAggregate,
  ExpectedIdentifierInAggregate,
  ExpectedOperatorAfterAggregate,
  ExpectedInOrLikeAfterNot,
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
  bool m_in_subquery = false;

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

  constexpr bool is_aggregate_func(TokenType type) {
    switch (type) {
    case TokenType::Count:
    case TokenType::Sum:
    case TokenType::Avg:
    case TokenType::Max:
    case TokenType::Min:
      return true;
    default:
      return false;
    }
  }

  // Parse: IN (SELECT ...) or IN (val1, val2, ...)
  constexpr void parse_in_clause(ParseResult &result) {
    advance(); // Consume 'IN'

    // Consume '('
    if (!match(TokenType::Lparen, result,
               SQLErrorKind::ExpectedLeftParenAfterIn)) {
      return;
    }

    if (peek().type == TokenType::Select) {
      auto res = parse_select(true);
      if (res.has_error) {
        result.has_error = true;
        result.error = res.error;
        return;
      }
    } else {
      // Parse hardcoded literal list: (1, 2, 'abc')
      while (true) {
        if (peek().type == TokenType::Number ||
            peek().type == TokenType::String ||
            peek().type == TokenType::PlaceHolder) {
          advance();
        } else {
          result.has_error = true;
          result.error = SQLErrorKind::ExpectedLiteralOrPlaceHolderInList;
          return;
        }

        if (peek().type == TokenType::Comma) {
          advance();
        } else {
          break; // End of list
        }
      }
    }

    if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
      return;
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

  constexpr void parse_not_expr(ParseResult &result) {
    advance();
    switch (peek().type) {
    case TokenType::In:
      parse_in_clause(result);
      break;
    case TokenType::Like:
      parse_like_clause(result);
      break;
    default: {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedInOrLikeAfterNot;
    }
    }
  }

  constexpr void parse_is_clause(ParseResult &result) {
    advance(); // consume is

    if (peek().type == TokenType::Null) {
      advance();
    } else if (peek().type == TokenType::Not) {
      advance();
      if (peek().type == TokenType::Null) {
        advance();
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedNullAfterNotInIsClause;
      }
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedNullAfterInInIsClause;
    }
  }

  constexpr void parse_aggregate_expr(ParseResult &result) {
    TokenType agg_type = peek().type;
    advance(); // Consume COUNT

    // Consume '('
    if (!match(TokenType::Lparen, result,
               SQLErrorKind::ExpectedLeftParenAfterAggregate)) {
      return;
    }

    if (agg_type == TokenType::Count && peek().type == TokenType::Star) {
      advance(); // Consume '*'
    } else if (peek().type == TokenType::Identifier) {
      advance(); // Consume identifier
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedIdentifierInAggregate;
      return;
    }

    // Consume ')'
    if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
      return;
    }
  }

  constexpr void parse_group_by_clause(ParseResult &result) {
    advance(); // consume GROUP

    if (!match(TokenType::By, result,
               SQLErrorKind::ExpectedByAfterGroupInGroupByClause)) {
      return;
    }

    if (!match(TokenType::Identifier, result,
               SQLErrorKind::ExpectedIdentifierAfterByInGroupByClause)) {
      return;
    }

    while (peek().type == TokenType::Comma) {
      advance(); // consume ','

      if (!match(TokenType::Identifier, result,
                 SQLErrorKind::ExpectedIdentifierAfterByInGroupByClause)) {
        return;
      }
    }
  }

  constexpr void parse_having_clause(ParseResult &result) {
    advance(); // consume HAVING
    parse_where_clause(result);
  }

  constexpr void parse_order_by_clause(ParseResult &result) {
    advance(); // consume ORDER

    if (!match(TokenType::By, result,
               SQLErrorKind::ExpectedByAfterOrderInOrderByClause)) {
      return;
    }

    while (true) {
      if (!match(TokenType::Identifier, result,
                 SQLErrorKind::ExpectedIdentifierAfterByInOrderByClause)) {
        return;
      }

      if (peek().type == TokenType::Asc || peek().type == TokenType::Desc) {
        advance();
      }

      if (peek().type == TokenType::Comma) {
        advance();
        continue;
      }

      break;
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
        auto res = parse_select(true);
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

      if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
        return;
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
        auto res = parse_select(true);
        if (res.has_error) {
          result.has_error = true;
          result.error = res.error;
          return;
        }

        if (!match(TokenType::Rparen, result,
                   SQLErrorKind::ExpectedRightParen)) {
          return;
        }
        return;
      }

      // Case B: (id > 10 AND ...) -> Normal precedence grouping
      parse_where_clause(result);
      if (result.has_error)
        return;

      if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
        return;
      }
      // exit the subquery
      return;
    }

    // 2. Match left-hand side operand (Must be a column identifier)
    if (is_aggregate_func(peek().type)) {
      parse_aggregate_expr(result); // Consume aggregate function
      if (result.has_error)
        return;

      if (!is_operator(peek().type)) {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedOperatorAfterAggregate;
        return;
      }
      advance(); // Consume binaray op

      if (!(peek().type == TokenType::Number ||
            peek().type == TokenType::PlaceHolder)) {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedLiteralOrPlaceHolder;
        return;
      }
      advance(); // Consume number or placeholder
      return;
    } else if (peek().type == TokenType::Identifier) {
      advance(); // Consume Identifier
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedIdentifierInWhereClause;
      return;
    }

    // 3. Dispatch to specific clause parsers based on the current lookahead
    // token
    switch (peek().type) {
    case TokenType::Not: {
      parse_not_expr(result);
      break;
    }
    case TokenType::In:
      parse_in_clause(result);
      break;

    case TokenType::Like:
      parse_like_clause(result);
      break;

    case TokenType::Between:
      parse_between_clause(result);
      break;

    case TokenType::Is:
      parse_is_clause(result);
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

  constexpr ParseResult parse_select(bool allow_rparen_end = false) {
    ParseResult result{};
    if (peek().type == TokenType::Select) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::UnknownBeginning;
      return result;
    }

    // Consume DISTINCT or ALL
    if (peek().type == TokenType::Distinct || peek().type == TokenType::All) {
      advance();
    }

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

    if (!match(TokenType::From, result, SQLErrorKind::ExpectedFrom)) {
      return result;
    }

    if (!match(TokenType::Identifier, result,
               SQLErrorKind::ExpectedTableAfterFrom)) {
      return result;
    }

    if (peek().type == TokenType::Where) {
      advance();
      parse_where_clause(result);
      if (result.has_error)
        return result;
    }

    bool has_group_by = false;

    if (peek().type == TokenType::Group) {
      parse_group_by_clause(result);
      if (result.has_error)
        return result;
      has_group_by = true;
    }

    if (peek().type == TokenType::Having) {
      if (!has_group_by) {
        result.has_error = true;
        result.error = SQLErrorKind::HavingWithoutGroupBy;
        return result;
      }

      parse_having_clause(result);
      if (result.has_error)
        return result;
    }

    if (peek().type == TokenType::Order) {
      parse_order_by_clause(result);
      if (result.has_error)
        return result;

      if (peek().type == TokenType::Asc || peek().type == TokenType::Desc) {
        advance();
      }
    }

    if (at_end()) {
      return result;
    }

    if (allow_rparen_end && peek().type == TokenType::Rparen) {
      return result;
    }

    result.has_error = true;
    result.error = SQLErrorKind::InvalidEnd;
    return result;
  }

  [[nodiscard]] constexpr ParseResult parse() {
    if (peek().type == TokenType::Select) {
      return parse_select(false);
    }
    return ParseResult::make_error();
  }
};

} // namespace ess::orm::sql
