#pragma once
#include "../parser_decl.hpp"

namespace ess::orm::sql {

template <std::size_t TokenCount>
constexpr ParseResult Parser<TokenCount>::parse_update() {
  ParseResult result;
  result.is_update = true;

  // UPDATE
  if (!match(TokenType::Update, result, SQLErrorKind::ExpectedUpdate)) {
    return result;
  }

  // table_name
  if (peek().type != TokenType::Identifier) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedTableAfterFrom;
    result.err_idx = peek().pos;
    return result;
  }
  result.update_stmt.table_name_pos = peek().pos;
  result.update_stmt.table_name_len = peek().len;
  advance();

  // SET
  if (!match(TokenType::Set, result, SQLErrorKind::ExpectedSet)) {
    return result;
  }

  // Parse SET clause
  parse_set_clause(result);
  if (result.has_error) {
    return result;
  }

  // Optional WHERE clause
  if (peek().type == TokenType::Where) {
    advance();
    result.update_stmt.where_pos = peek().pos;
    
    // Parse WHERE expression
    parse_where_clause(result);
    if (result.has_error) {
      return result;
    }
    
    // Calculate WHERE expression length
    result.update_stmt.where_len = previous().pos + previous().len - result.update_stmt.where_pos;
  }

  return result;
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_set_clause(ParseResult &result) {
  bool first = true;
  
  while (!at_end() && peek().type != TokenType::Where) {
    if (!first) {
      if (!match(TokenType::Comma, result, SQLErrorKind::ExpectedAssignment)) {
        return;
      }
    }
    first = false;

    // column_name
    if (peek().type != TokenType::Identifier) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedIdentifier;
      result.err_idx = peek().pos;
      return;
    }

    // Store column info
    if (result.update_stmt.set_count < result.update_stmt.set_columns.size()) {
      Column col;
      col.pos = peek().pos;
      col.len = peek().len;
      result.update_stmt.set_columns[result.update_stmt.set_count] = col;
      result.update_stmt.set_count++;
    }
    advance();

    // =
    if (!match(TokenType::Eq, result, SQLErrorKind::ExpectedAssignment)) {
      return;
    }

    // value expression
    if (!parse_update_value(result)) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedValue;
      result.err_idx = peek().pos;
      return;
    }
  }

  if (result.update_stmt.set_count == 0) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedAssignment;
    result.err_idx = peek().pos;
  }
}

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::parse_update_value(ParseResult &result) {
  return parse_update_value_expr(result, 0);
}

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::parse_update_value_expr(ParseResult &result,
                                                          int min_precedence) {
  if (!parse_update_primary_expr(result)) {
    return false;
  }

  while (!at_end()) {
    Token op = peek();
    if (!is_arithmetic_operator(op.type)) {
      break;
    }

    int precedence = get_operator_precedence(op.type);
    if (precedence < min_precedence) {
      break;
    }

    advance(); // consume operator

    // Parse right operand
    if (!parse_update_value_expr(result, precedence + 1)) {
      return false;
    }
  }

  return true;
}

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::parse_update_primary_expr(ParseResult &result) {
  Token token = peek();
  
  switch (token.type) {
  case TokenType::Identifier: {
    // Could be column name or function call
    advance();
    
    // Check if it's a function call
    if (peek().type == TokenType::Lparen) {
      advance(); // consume '('
      
      // Parse function arguments
      if (peek().type != TokenType::Rparen) {
        bool first = true;
        while (!at_end() && peek().type != TokenType::Rparen) {
          if (!first) {
            if (!match(TokenType::Comma, result, SQLErrorKind::ExpectedValue)) {
              return false;
            }
          }
          first = false;
          
          if (!parse_update_value_expr(result, 0)) {
            return false;
          }
        }
      }
      
      if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
        return false;
      }
    }
    return true;
  }
  
  case TokenType::Number:
  case TokenType::String:
  case TokenType::True:
  case TokenType::False:
  case TokenType::Null:
    advance();
    return true;
    
  case TokenType::PlaceHolder:
    // Record placeholder for semantic analysis
    result.semantic_result.add_placeholder(peek().pos);
    advance();
    return true;
    
  case TokenType::Default:
    advance();
    return true;
    
  case TokenType::Lparen: {
    advance(); // consume '('
    
    // Check if it's a subquery
    if (peek().type == TokenType::Select) {
      // Parse subquery
      ParseResult subquery_result = parse_compound_select(true);
      if (subquery_result.has_error) {
        result.has_error = true;
        result.error = subquery_result.error;
        result.err_idx = subquery_result.err_idx;
        return false;
      }
    } else {
      // Parse expression in parentheses
      if (!parse_update_value_expr(result, 0)) {
        return false;
      }
    }
    
    if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
      return false;
    }
    return true;
  }
  
  case TokenType::Plus:
  case TokenType::Minus: {
    // Unary plus/minus
    advance();
    return parse_update_primary_expr(result);
  }
  
  default:
    return false;
  }
}

} // namespace ess::orm::sql