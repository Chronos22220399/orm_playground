#pragma once
#include "../parser_decl.hpp"

namespace ess::orm::sql {

template <std::size_t TokenCount>
constexpr ParseResult Parser<TokenCount>::parse_insert() {
  ParseResult result;
  result.is_insert = true;

  // INSERT
  if (!match(TokenType::Insert, result, SQLErrorKind::ExpectedInsert)) {
    return result;
  }

  // INTO
  if (!match(TokenType::Into, result, SQLErrorKind::ExpectedInto)) {
    return result;
  }

  // table_name
  if (peek().type != TokenType::Identifier) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedTableAfterFrom;
    result.err_idx = peek().pos;
    return result;
  }
  result.insert_stmt.table_name_pos = peek().pos;
  result.insert_stmt.table_name_len = peek().len;
  advance();

  // Optional column list
  if (peek().type == TokenType::Lparen) {
    advance(); // consume '('

    // Parse column list
    while (true) {
      if (peek().type != TokenType::Identifier) {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedIdentifier;
        result.err_idx = peek().pos;
        return result;
      }

      // Add column to insert_stmt
      if (result.insert_stmt.column_count < result.insert_stmt.columns.size()) {
        Column col;
        col.pos = peek().pos;
        col.len = peek().len;
        result.insert_stmt.columns[result.insert_stmt.column_count++] = col;
      }
      advance();

      if (peek().type == TokenType::Comma) {
        advance();
        continue;
      } else if (peek().type == TokenType::Rparen) {
        advance();
        break;
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedRightParen;
        result.err_idx = peek().pos;
        return result;
      }
    }
  }

  // VALUES or SELECT
  if (peek().type == TokenType::Values) {
    return parse_values_clause(result);
  } else if (peek().type == TokenType::Select) {
    result.insert_stmt.is_select = true;
    auto select_result = parse_select(false);
    if (select_result.has_error) {
      result.has_error = true;
      result.error = select_result.error;
      result.err_idx = select_result.err_idx;
    }
    return result;
  } else {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedValuesOrSelect;
    result.err_idx = peek().pos;
    return result;
  }
}

template <std::size_t TokenCount>
constexpr ParseResult
Parser<TokenCount>::parse_values_clause(ParseResult &result) {
  // VALUES
  advance(); // consume VALUES

  // Parse one or more value lists
  while (true) {
    // Left parenthesis
    if (!match(TokenType::Lparen, result,
               SQLErrorKind::ExpectedLeftParenAfterIn)) {
      return result;
    }

    // Parse values inside parentheses
    while (true) {
      // Parse a single value (could be literal, expression, function call,
      // etc.)
      if (!parse_insert_value(result)) {
        return result;
      }

      if (peek().type == TokenType::Comma) {
        advance();
        continue;
      } else if (peek().type == TokenType::Rparen) {
        advance();
        break;
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedRightParen;
        result.err_idx = peek().pos;
        return result;
      }
    }

    // Check if there are more value lists
    if (peek().type == TokenType::Comma) {
      advance();
      continue;
    } else {
      break;
    }
  }

  return result;
}

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::parse_insert_value(ParseResult &result) {
  // Parse a value expression (could be simple literal or complex expression)
  return parse_insert_value_expr(result, 0);
}

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::parse_insert_value_expr(ParseResult &result,
                                                           int min_precedence) {
  // Parse primary expression
  if (!parse_insert_primary_expr(result)) {
    return false;
  }

  // Parse binary operators
  while (true) {
    TokenType op = peek().type;
    int precedence = get_operator_precedence(op);

    if (precedence < min_precedence) {
      break;
    }

    if (op == TokenType::Plus || op == TokenType::Minus ||
        op == TokenType::Star || op == TokenType::Slash ||
        op == TokenType::Percent) {
      advance(); // consume operator
      if (!parse_insert_value_expr(result, precedence + 1)) {
        return false;
      }
    } else {
      break;
    }
  }

  return true;
}

template <std::size_t TokenCount>
constexpr bool
Parser<TokenCount>::parse_insert_primary_expr(ParseResult &result) {
  // Accept: number, string, placeholder, true, false, null, DEFAULT
  if (peek().type == TokenType::Number || peek().type == TokenType::String ||
      peek().type == TokenType::PlaceHolder || peek().type == TokenType::True ||
      peek().type == TokenType::False || peek().type == TokenType::Null ||
      peek().type == TokenType::Default) {
    // Record placeholder for semantic analysis
    if (peek().type == TokenType::PlaceHolder) {
      result.semantic_result.add_placeholder(peek().pos);
    }
    advance();
    return true;
  } else if (peek().type == TokenType::Lparen) {
    // Could be expression or subquery
    // Check if it's a subquery (starts with SELECT)
    std::size_t saved_pos = m_pos;
    advance(); // consume '('

    // Skip whitespace tokens if any
    while (peek().type == TokenType::Select) {
      // It's a subquery - parse it
      auto select_result = parse_select(true); // allow_rparen_end = true
      if (select_result.has_error) {
        result.has_error = true;
        result.error = select_result.error;
        result.err_idx = select_result.err_idx;
        return false;
      }

      // Consume the closing ')'
      if (peek().type == TokenType::Rparen) {
        advance();
        return true;
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedRightParen;
        result.err_idx = peek().pos;
        return false;
      }
    }

    // Not a subquery, restore position and parse as expression
    m_pos = saved_pos;
    advance(); // consume '('
    if (!parse_insert_value_expr(result, 0)) {
      return false;
    }
    if (peek().type != TokenType::Rparen) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedRightParen;
      result.err_idx = peek().pos;
      return false;
    }
    advance(); // consume ')'
    return true;
  } else if (peek().type == TokenType::Identifier) {
    // Could be function call or column reference
    advance(); // consume identifier

    // Check for function call
    if (peek().type == TokenType::Lparen) {
      advance(); // consume '('
      // Parse function arguments
      while (true) {
        if (peek().type == TokenType::Rparen) {
          advance(); // consume ')'
          break;
        }

        if (!parse_insert_value_expr(result, 0)) {
          return false;
        }

        if (peek().type == TokenType::Comma) {
          advance(); // consume ','
          continue;
        } else if (peek().type == TokenType::Rparen) {
          advance(); // consume ')'
          break;
        } else {
          result.has_error = true;
          result.error = SQLErrorKind::ExpectedRightParen;
          result.err_idx = peek().pos;
          return false;
        }
      }
    }
    return true;
  } else if (peek().type == TokenType::Plus ||
             peek().type == TokenType::Minus) {
    // Unary operator
    advance();
    return parse_insert_primary_expr(result);
  } else {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedValue;
    result.err_idx = peek().pos;
    return false;
  }
}

template <std::size_t TokenCount>
constexpr int Parser<TokenCount>::get_operator_precedence(TokenType op) {
  switch (op) {
  case TokenType::Star:
  case TokenType::Slash:
  case TokenType::Percent:
    return 3;
  case TokenType::Plus:
  case TokenType::Minus:
    return 2;
  default:
    return -1; // Not an operator
  }
}

} // namespace ess::orm::sql
