#pragma once
#include "../parser_decl.hpp"

namespace ess::orm::sql {

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::at_end() const {
  return m_pos >= TokenCount || m_tokens[m_pos].type == TokenType::End;
}

template <std::size_t TokenCount>
constexpr Token Parser<TokenCount>::advance() {
  if (at_end())
    return make_end();
  return m_tokens[m_pos++];
}

template <std::size_t TokenCount>
constexpr Token Parser<TokenCount>::peek() const {
  if (at_end())
    return make_end();
  return m_tokens[m_pos];
}

template <std::size_t TokenCount>
constexpr Token Parser<TokenCount>::peek_next() const {
  if (m_pos + 1 >= TokenCount || m_tokens[m_pos + 1].type == TokenType::End)
    return make_end();
  return m_tokens[m_pos + 1];
}

template <std::size_t TokenCount>
constexpr Token Parser<TokenCount>::previous() const {
  if (m_pos == 0)
    return make_end();
  return m_tokens[m_pos - 1];
}

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::match(TokenType type, ParseResult &result,
                                         SQLErrorKind error) {
  if (peek().type == type) {
    advance();
    return true;
  }
  result.has_error = true;
  result.error = error;
  result.err_idx = peek().pos;
  return false;
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_columns(ParseResult &result) {
  bool first_vis = true;
  
  while (true) {
    // Check if we have a select item
    if (peek().type == TokenType::Star) {
      // Handle SELECT *
      result.is_star = true;
      Column col;
      col.pos = peek().pos;
      col.len = peek().len;
      result.add_column(col);
      advance();
      
      if (first_vis) {
        result.column_start = col.pos;
        first_vis = false;
      }
      
    } else if (is_aggregate_func(peek().type)) {
      // Handle aggregate functions like COUNT(*), SUM(id), etc.
      result.has_aggregate = true;
      Token start_token = peek();
      
      if (first_vis) {
        result.column_start = start_token.pos;
        first_vis = false;
      }
      
      // Parse the aggregate expression
      parse_aggregate_expr(result);
      if (result.has_error) return;
      
      // Create column entry for aggregate expression
      Column col;
      col.pos = start_token.pos;
      // Calculate length from start to current position (excluding alias)
      if (!at_end()) {
        Token last_token = previous();
        col.len = (last_token.pos + last_token.len) - col.pos;
      } else {
        col.len = 1; // fallback
      }
      
      // Check for alias
      if (peek().type == TokenType::As) {
        advance();
        if (peek().type == TokenType::Identifier) {
          Token alias_token = peek();
          col.alias_pos = alias_token.pos;
          col.alias_len = alias_token.len;
          advance();
        }
      } else if (peek().type == TokenType::Identifier) {
        // Potential implicit alias (e.g., "SELECT column alias")
        // In SQL, a column alias can be specified without AS
        // Check if this identifier is likely an alias by looking at next token
        TokenType next_type = peek_next().type;
        // Alias is likely if followed by comma, FROM, end of query, or right parenthesis
        if (next_type == TokenType::Comma || next_type == TokenType::From ||
            next_type == TokenType::End || next_type == TokenType::Rparen) {
          Token alias_token = peek();
          col.alias_pos = alias_token.pos;
          col.alias_len = alias_token.len;
          advance();
        }
        // Otherwise, leave the identifier for parse_columns to handle as next column
      }
      
      result.add_column(col);
      
    } else if (peek().type == TokenType::Identifier ||
               peek().type == TokenType::Number ||
               peek().type == TokenType::String ||
               peek().type == TokenType::PlaceHolder ||
               peek().type == TokenType::True ||
               peek().type == TokenType::False ||
               peek().type == TokenType::Null ||
               peek().type == TokenType::Lparen ||
               peek().type == TokenType::Plus ||
               peek().type == TokenType::Minus) {
      // Handle expression (column, literal, arithmetic expression, etc.)
      parse_select_item(result);
      if (result.has_error) return;
      
      if (first_vis) {
        // column_start already set in parse_select_item
        first_vis = false;
      }
      
    } else {
      // No more select items
      break;
    }
    
    // Check for comma to continue with next column
    if (peek().type == TokenType::Comma) {
      advance();
    } else {
      break;
    }
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_select_item(ParseResult &result) {
  // Record start position for column_start if not already set
  if (result.column_start == 0) {
    result.column_start = peek().pos;
  }
  
  // Create column entry
  Column col;
  col.pos = peek().pos;
  std::size_t start_pos = col.pos;
  
  // Parse unary plus/minus if present
  if (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
    advance();
  }
  
  // Parse primary expression
  if (peek().type == TokenType::Identifier) {
    // Check if this is a function call (identifier followed by left parenthesis)
    if (peek_next().type == TokenType::Lparen) {
      // Function call - skip function name and arguments
      advance(); // consume function name
      advance(); // consume left parenthesis
      // Skip arguments until matching right parenthesis
      int paren_depth = 1;
      while (paren_depth > 0 && !at_end()) {
        if (peek().type == TokenType::Lparen) {
          paren_depth++;
        } else if (peek().type == TokenType::Rparen) {
          paren_depth--;
        }
        advance();
      }
      // Note: don't set has_left_column for function calls
    } else if (peek_next().type == TokenType::Dot) {
      // First identifier is table name
      Token table_token = peek();
      advance(); // consume table identifier
      advance(); // consume dot
      if (peek().type == TokenType::Identifier) {
        // Check if this is a function call with table/schema prefix
        if (peek_next().type == TokenType::Lparen) {
          // Function call with prefix, e.g., schema.func(args)
          advance(); // consume function name
          advance(); // consume left parenthesis
          // Skip arguments
          int paren_depth = 1;
          while (paren_depth > 0 && !at_end()) {
            if (peek().type == TokenType::Lparen) {
              paren_depth++;
            } else if (peek().type == TokenType::Rparen) {
              paren_depth--;
            }
            advance();
          }
          // Note: don't set has_left_column for function calls
        } else {
          // Column identifier
          result.has_left_column = true;
          advance();
          col.has_table_prefix = true;
          col.table_pos = table_token.pos;
          col.table_len = table_token.len;
        }
      } else {
        // Error: expected column name after dot
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedIdentifier;
        result.err_idx = peek().pos;
        return;
      }
    } else {
      // Simple column identifier
      result.has_left_column = true;
      advance();
    }
  } else if (peek().type == TokenType::Number ||
             peek().type == TokenType::String ||
             peek().type == TokenType::PlaceHolder ||
             peek().type == TokenType::True ||
             peek().type == TokenType::False ||
             peek().type == TokenType::Null) {
    // Literal value
    advance();
  } else if (peek().type == TokenType::Lparen) {
    // Parenthesized expression
    advance();
    // Parse expression inside parentheses recursively
    // For now, skip until matching right parenthesis
    int paren_depth = 1;
    while (paren_depth > 0 && !at_end()) {
      if (peek().type == TokenType::Lparen) {
        paren_depth++;
      } else if (peek().type == TokenType::Rparen) {
        paren_depth--;
      }
      advance();
    }
  } else {
    // Unexpected token
    result.has_error = true;
    result.error = SQLErrorKind::InvalidSelectItem;
    result.err_idx = peek().pos;
    return;
  }
  
  // Check for arithmetic operator
  if (is_arithmetic_operator(peek().type)) {
    result.has_arithmetic = true;
    parse_binary_operator_clause(result);
    if (result.has_error) return;
  }
  
  // Calculate column length (approximate)
  // Use last consumed token position and length
  if (!at_end()) {
    Token last_token = previous();
    col.len = (last_token.pos + last_token.len) - col.pos;
  } else {
    col.len = 1; // fallback
  }
  
  // Check for alias
  if (peek().type == TokenType::As) {
    advance();
    if (peek().type == TokenType::Identifier) {
      Token alias_token = peek();
      col.alias_pos = alias_token.pos;
      col.alias_len = alias_token.len;
      advance();
    }
  } else if (peek().type == TokenType::Identifier) {
    // Potential implicit alias (e.g., "SELECT column alias")
    // In SQL, a column alias can be specified without AS
    // Check if this identifier is likely an alias by looking at next token
    TokenType next_type = peek_next().type;
    // Alias is likely if followed by comma, FROM, end of query, or right parenthesis
    if (next_type == TokenType::Comma || next_type == TokenType::From ||
        next_type == TokenType::End || next_type == TokenType::Rparen) {
      Token alias_token = peek();
      col.alias_pos = alias_token.pos;
      col.alias_len = alias_token.len;
      advance();
    }
    // Otherwise, leave the identifier for parse_columns to handle as next column
  }
  
  // Add column to result
  result.add_column(col);
}

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::is_operator(TokenType type) {
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

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::is_aggregate_func(TokenType type) {
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

template <std::size_t TokenCount>
constexpr bool Parser<TokenCount>::is_arithmetic_operator(TokenType type) {
  switch (type) {
  case TokenType::Plus:
  case TokenType::Minus:
  case TokenType::Star:
  case TokenType::Slash:
  case TokenType::Percent:
    return true;
  default:
    return false;
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_in_clause(ParseResult &result) {
  advance();

  if (!match(TokenType::Lparen, result,
             SQLErrorKind::ExpectedLeftParenAfterIn)) {
    return;
  }

  if (peek().type == TokenType::Select) {
    // Parse subquery with proper parenthesis nesting
    int paren_depth = 0;
    while (!at_end()) {
      if (peek().type == TokenType::Lparen) {
        paren_depth++;
      } else if (peek().type == TokenType::Rparen) {
        if (paren_depth == 0) {
          // This is the closing paren of the IN clause
          break;
        }
        paren_depth--;
      }
      advance();
    }
    // Now peek() should be the closing Rparen of the IN clause
  } else {
    while (true) {
      if (peek().type == TokenType::Number || peek().type == TokenType::String ||
          peek().type == TokenType::PlaceHolder || peek().type == TokenType::Identifier) {
        advance();
      } else {
        break;
      }

      if (peek().type == TokenType::Comma) {
        advance();
      } else {
        break;
      }
    }
  }

  if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
    return;
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_like_clause(ParseResult &result) {
  advance();

  if (peek().type == TokenType::String) {
    advance();
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_between_clause(ParseResult &result) {
  if (peek().type == TokenType::Not) {
    advance();
  }

  while (peek().type != TokenType::And && peek().type != TokenType::End &&
         peek().type != TokenType::Rparen) {
    advance();
  }

  if (peek().type == TokenType::And) {
    advance();
  }

  while (peek().type != TokenType::End && peek().type != TokenType::Rparen &&
         peek().type != TokenType::Group && peek().type != TokenType::Order &&
         peek().type != TokenType::Limit) {
    advance();
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_not_expr(ParseResult &result) {
  advance();
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
  case TokenType::Exists:
    parse_exists_clause(result);
    break;
  default:
    // Generic NOT expr: NOT ( ... )
    if (peek().type == TokenType::Lparen) {
      advance(); // consume '('
      int paren_count = 1;
      while (!at_end() && paren_count > 0) {
        if (peek().type == TokenType::Lparen) {
          paren_count++;
        } else if (peek().type == TokenType::Rparen) {
          paren_count--;
          if (paren_count == 0) {
            advance(); // consume the final ')'
            break;
          }
        }
        advance();
      }
      if (paren_count != 0) {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedRightParen;
        result.err_idx = peek().pos;
      }
    } else {
      // NOT followed by something else - could be a unary NOT before expression
      // For now, treat as generic NOT expr and consume until separator
      // This is not standard SQL but allows simple cases
      // We'll just advance and let parse_primary_expr handle the rest
      // Actually, we should not advance here, let parse_primary_expr continue
      // But we already consumed NOT token, so we need to parse the following expression
      // For simplicity, we'll just return and let parse_primary_expr handle it
      // parse_primary_expr will be called after parse_not_expr returns
    }
    break;
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_exists_clause(ParseResult &result) {
  advance(); // consume EXISTS

  // Expect '('
  if (peek().type != TokenType::Lparen) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedLeftParenAfterExists;
    result.err_idx = peek().pos;
    return;
  }
  advance();

  // Expect SELECT
  if (peek().type != TokenType::Select) {
    result.has_error = true;
    result.error = SQLErrorKind::UnknownError;
    result.err_idx = peek().pos;
    return;
  }

  // Skip entire subquery until matching ')'
  int paren_count = 1; // already consumed one '('
  while (!at_end() && paren_count > 0) {
    if (peek().type == TokenType::Lparen) {
      paren_count++;
    } else if (peek().type == TokenType::Rparen) {
      paren_count--;
      if (paren_count == 0) {
        advance(); // consume the final ')'
        break;
      }
    }
    advance();
  }

  // If we exited due to end of input, it's an error
  if (paren_count != 0) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedRightParen;
    result.err_idx = peek().pos;
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_is_clause(ParseResult &result) {
  // Check left operand before consuming IS
  Token left_operand = previous();
  
  // Check if left operand is invalid for IS NULL / IS NOT NULL
  if (left_operand.type == TokenType::Number || 
      left_operand.type == TokenType::String ||
      left_operand.type == TokenType::Null ||
      left_operand.type == TokenType::True ||
      left_operand.type == TokenType::False) {
    result.has_error = true;
    result.error = SQLErrorKind::InvalidIsNullOperand;
    result.err_idx = left_operand.pos;
    return;
  }
  
  advance();

  if (peek().type == TokenType::Null) {
    advance();
  } else if (peek().type == TokenType::Not) {
    advance();
    if (peek().type == TokenType::Null) {
      advance();
    }
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_aggregate_expr(ParseResult &result) {
  advance();

  if (!match(TokenType::Lparen, result,
             SQLErrorKind::ExpectedLeftParenAfterAggregate)) {
    return;
  }

  // Check for DISTINCT modifier
  if (peek().type == TokenType::Distinct) {
    advance();
  }

  if (peek().type == TokenType::Identifier || peek().type == TokenType::Star) {
    advance();
  }

  if (!match(TokenType::Rparen, result, SQLErrorKind::ExpectedRightParen)) {
    return;
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_group_by_item(ParseResult &result) {
  // Parse unary plus/minus if present
  if (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
    advance();
  }
  
  // Parse primary expression
  if (peek().type == TokenType::Identifier) {
    // Check if this is a function call (identifier followed by left parenthesis)
    if (peek_next().type == TokenType::Lparen) {
      // Function call - skip function name and arguments
      advance(); // consume function name
      advance(); // consume left parenthesis
      // Skip arguments until matching right parenthesis
      int paren_depth = 1;
      while (paren_depth > 0 && !at_end()) {
        if (peek().type == TokenType::Lparen) {
          paren_depth++;
        } else if (peek().type == TokenType::Rparen) {
          paren_depth--;
        }
        advance();
      }
    } else if (peek_next().type == TokenType::Dot) {
      // First identifier is table name
      advance(); // consume table identifier
      advance(); // consume dot
      if (peek().type == TokenType::Identifier) {
        // Check if this is a function call with table/schema prefix
        if (peek_next().type == TokenType::Lparen) {
          // Function call with prefix, e.g., schema.func(args)
          advance(); // consume function name
          advance(); // consume left parenthesis
          // Skip arguments
          int paren_depth = 1;
          while (paren_depth > 0 && !at_end()) {
            if (peek().type == TokenType::Lparen) {
              paren_depth++;
            } else if (peek().type == TokenType::Rparen) {
              paren_depth--;
            }
            advance();
          }
        } else {
          // Column identifier
          advance();
        }
      } else {
        // Error: expected column name after dot
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedIdentifier;
        result.err_idx = peek().pos;
        return;
      }
    } else {
      // Simple column identifier
      advance();
    }
  } else if (peek().type == TokenType::Number ||
             peek().type == TokenType::String ||
             peek().type == TokenType::PlaceHolder ||
             peek().type == TokenType::True ||
             peek().type == TokenType::False ||
             peek().type == TokenType::Null) {
    // Literal value
    advance();
  } else if (peek().type == TokenType::Lparen) {
    // Parenthesized expression
    advance();
    // Parse expression inside parentheses recursively
    // For now, skip until matching right parenthesis
    int paren_depth = 1;
    while (paren_depth > 0 && !at_end()) {
      if (peek().type == TokenType::Lparen) {
        paren_depth++;
      } else if (peek().type == TokenType::Rparen) {
        paren_depth--;
      }
      advance();
    }
  } else {
    // Unexpected token
    result.has_error = true;
    result.error = SQLErrorKind::InvalidSelectItem;
    result.err_idx = peek().pos;
    return;
  }
  
  // Check for arithmetic operator
  if (is_arithmetic_operator(peek().type)) {
    parse_binary_operator_clause(result);
    if (result.has_error) return;
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_group_by_clause(ParseResult &result) {
  advance();

  if (!match(TokenType::By, result,
             SQLErrorKind::ExpectedByAfterGroupInGroupByClause)) {
    return;
  }

  // Parse GROUP BY expressions
  while (true) {
    // Parse one group by expression
    parse_group_by_item(result);
    if (result.has_error) return;
    
    // Check for comma to continue with next expression
    if (peek().type == TokenType::Comma) {
      advance();
    } else {
      break;
    }
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_having_clause(ParseResult &result) {
  advance();
  while (peek().type != TokenType::Order && peek().type != TokenType::Limit &&
         peek().type != TokenType::End) {
    advance();
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_order_by_item(ParseResult &result) {
  // Check for aggregate function
  if (is_aggregate_func(peek().type)) {
    parse_aggregate_expr(result);
    return;
  }
  
  // Parse unary plus/minus if present
  if (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
    advance();
  }
  
  // Parse primary expression
  if (peek().type == TokenType::Identifier) {
    // Check if this is a function call (identifier followed by left parenthesis)
    if (peek_next().type == TokenType::Lparen) {
      // Function call - skip function name and arguments
      advance(); // consume function name
      advance(); // consume left parenthesis
      // Skip arguments until matching right parenthesis
      int paren_depth = 1;
      while (paren_depth > 0 && !at_end()) {
        if (peek().type == TokenType::Lparen) {
          paren_depth++;
        } else if (peek().type == TokenType::Rparen) {
          paren_depth--;
        }
        advance();
      }
    } else if (peek_next().type == TokenType::Dot) {
      // First identifier is table name
      advance(); // consume table identifier
      advance(); // consume dot
      if (peek().type == TokenType::Identifier) {
        // Check if this is a function call with table/schema prefix
        if (peek_next().type == TokenType::Lparen) {
          // Function call with prefix, e.g., schema.func(args)
          advance(); // consume function name
          advance(); // consume left parenthesis
          // Skip arguments
          int paren_depth = 1;
          while (paren_depth > 0 && !at_end()) {
            if (peek().type == TokenType::Lparen) {
              paren_depth++;
            } else if (peek().type == TokenType::Rparen) {
              paren_depth--;
            }
            advance();
          }
        } else {
          // Column identifier
          advance();
        }
      } else {
        // Error: expected column name after dot
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedIdentifier;
        result.err_idx = peek().pos;
        return;
      }
    } else {
      // Simple column identifier
      advance();
    }
  } else if (peek().type == TokenType::Number ||
             peek().type == TokenType::String ||
             peek().type == TokenType::PlaceHolder ||
             peek().type == TokenType::True ||
             peek().type == TokenType::False ||
             peek().type == TokenType::Null) {
    // Literal value
    advance();
  } else if (peek().type == TokenType::Lparen) {
    // Parenthesized expression
    advance();
    // Parse expression inside parentheses recursively
    // For now, skip until matching right parenthesis
    int paren_depth = 1;
    while (paren_depth > 0 && !at_end()) {
      if (peek().type == TokenType::Lparen) {
        paren_depth++;
      } else if (peek().type == TokenType::Rparen) {
        paren_depth--;
      }
      advance();
    }
  } else {
    // Unexpected token
    result.has_error = true;
    result.error = SQLErrorKind::InvalidSelectItem;
    result.err_idx = peek().pos;
    return;
  }
  
  // Check for arithmetic operator
  if (is_arithmetic_operator(peek().type)) {
    parse_binary_operator_clause(result);
    if (result.has_error) return;
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_order_by_clause(ParseResult &result) {
  advance();

  if (!match(TokenType::By, result,
             SQLErrorKind::ExpectedByAfterOrderInOrderByClause)) {
    return;
  }

  while (peek().type != TokenType::Limit && peek().type != TokenType::End) {
    // Parse one order by expression
    parse_order_by_item(result);
    if (result.has_error) return;
    
    // Check for ASC/DESC
    if (peek().type == TokenType::Asc || peek().type == TokenType::Desc) {
      advance();
    }
    
    // Check for comma to continue with next order item
    if (peek().type == TokenType::Comma) {
      advance();
    } else {
      break;
    }
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_binary_operator_clause(
    ParseResult &result) {
  // Skip the binary operator token
  advance();
  
  // Check if we have a valid operand after the binary operator
  // Valid operand starters: identifier, number, string, placeholder, boolean, NULL, left paren, unary plus/minus
  bool has_operand = false;
  
  // Handle unary plus/minus
  if (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
    advance();
  }
  
  // Parse the operand
  if (peek().type == TokenType::Identifier) {
    has_operand = true;
    advance();
    
    // Skip table prefix if present
    while (peek().type == TokenType::Dot) {
      advance();
      if (peek().type == TokenType::Identifier) {
        advance();
      }
    }
  } else if (peek().type == TokenType::Number ||
             peek().type == TokenType::String ||
             peek().type == TokenType::PlaceHolder ||
             peek().type == TokenType::True ||
             peek().type == TokenType::False ||
             peek().type == TokenType::Null) {
    has_operand = true;
    advance();
  } else if (peek().type == TokenType::Lparen) {
    has_operand = true;
    // Skip parenthesized expression
    advance();
    int paren_depth = 1;
    while (paren_depth > 0 && !at_end()) {
      if (peek().type == TokenType::Lparen) {
        paren_depth++;
      } else if (peek().type == TokenType::Rparen) {
        paren_depth--;
      }
      advance();
    }
  }
  
  // If no valid operand found after binary operator, report error
  if (!has_operand) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedLiteralOrPlaceHolder;
    result.err_idx = peek().pos;
    return;
  }
  
  // Check for chained arithmetic operator (e.g., a + b + c)
  if (is_arithmetic_operator(peek().type)) {
    result.has_arithmetic = true;
    advance();
    
    // Check for unary plus/minus before operand
    if (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
      advance();
    }
    
    // Parse the next operand for chained operator
    // Valid operand starters: identifier, number, string, placeholder, boolean, NULL, left paren
    if (peek().type == TokenType::Identifier ||
        peek().type == TokenType::Number ||
        peek().type == TokenType::String ||
        peek().type == TokenType::PlaceHolder ||
        peek().type == TokenType::True ||
        peek().type == TokenType::False ||
        peek().type == TokenType::Null ||
        peek().type == TokenType::Lparen) {
      advance();
      
      // If it's a left parenthesis, skip the entire parenthesized expression
      if (previous().type == TokenType::Lparen) {
        int paren_depth = 1;
        while (paren_depth > 0 && !at_end()) {
          if (peek().type == TokenType::Lparen) {
            paren_depth++;
          } else if (peek().type == TokenType::Rparen) {
            paren_depth--;
          }
          advance();
        }
      }
    } else {
      // Missing operand after chained operator
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedLiteralOrPlaceHolder;
      result.err_idx = peek().pos;
      return;
    }
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_arithmetic_expr(ParseResult &result) {
  if (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
    advance();
  }

  if (peek().type == TokenType::Number || peek().type == TokenType::PlaceHolder) {
    advance();
  } else if (peek().type == TokenType::Identifier) {
    result.has_left_column = true;
    advance();
    if (peek().type == TokenType::Dot) {
      advance();
      if (peek().type == TokenType::Identifier) {
        advance();
      }
    }
  }

  if (is_arithmetic_operator(peek().type)) {
    parse_binary_operator_clause(result);
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_primary_expr(ParseResult &result) {
  if (peek().type == TokenType::Not) {
    parse_not_expr(result);
    return;
  }
  
  if (peek().type == TokenType::Exists) {
    parse_exists_clause(result);
    return;
  }
  
  if (peek().type == TokenType::In) {
    parse_in_clause(result);
    return;
  }
  
  if (peek().type == TokenType::Like) {
    parse_like_clause(result);
    return;
  }
  
  if (peek().type == TokenType::Between) {
    parse_between_clause(result);
    return;
  }
  
  if (peek().type == TokenType::Is) {
    parse_is_clause(result);
    return;
  }
  
  // Check for missing operator: id NULL, id 1, id 'text', id other_column, id (SELECT...), etc.
  // But allow id NOT BETWEEN, id NOT IN, id NOT LIKE, id NOT EXISTS
  if (peek().type == TokenType::Identifier) {
    TokenType next_type = peek_next().type;
    bool should_reject = false;
    
    if (next_type == TokenType::Number || next_type == TokenType::String ||
        next_type == TokenType::PlaceHolder || next_type == TokenType::True ||
        next_type == TokenType::False || next_type == TokenType::Null ||
        next_type == TokenType::Identifier || next_type == TokenType::Lparen ||
        next_type == TokenType::Exists) {
      should_reject = true;
    } else if (next_type == TokenType::Not) {
      // Check if NOT is followed by BETWEEN, IN, LIKE, EXISTS
      // If not, then it's missing operator
      if (m_pos + 2 < TokenCount) {
        TokenType after_not = m_tokens[m_pos + 2].type;
        if (after_not != TokenType::Between && after_not != TokenType::In &&
            after_not != TokenType::Like && after_not != TokenType::Exists) {
          should_reject = true;
        }
        // If after_not is Between/In/Like/Exists, it will be handled later
      } else {
        // NOT at end, missing operator
        should_reject = true;
      }
    }
    
    if (should_reject) {
      result.has_error = true;
      result.error = SQLErrorKind::MissingOperator;
      result.err_idx = peek_next().pos;
      return;
    }
  }
  
  // Default: parse expression with possible operators
  while (peek().type != TokenType::And && peek().type != TokenType::Or &&
         peek().type != TokenType::End && peek().type != TokenType::Group &&
         peek().type != TokenType::Order && peek().type != TokenType::Limit &&
         peek().type != TokenType::Rparen) {
    
    // Check for operators that need special handling
    if (peek().type == TokenType::In) {
      parse_in_clause(result);
      if (result.has_error) return;
      continue;
    }
    
    if (peek().type == TokenType::Like) {
      parse_like_clause(result);
      if (result.has_error) return;
      continue;
    }
    
    if (peek().type == TokenType::Between) {
      parse_between_clause(result);
      if (result.has_error) return;
      continue;
    }
    
    if (peek().type == TokenType::Is) {
      parse_is_clause(result);
      if (result.has_error) return;
      continue;
    }
    
    // Check for comparison operators
    if (is_operator(peek().type)) {
      // Save operator type for semantic checks
      TokenType op = peek().type;
      // Skip the operator
      advance();
      
      // Parse the right-hand side
      if (peek().type == TokenType::String || peek().type == TokenType::Number ||
          peek().type == TokenType::PlaceHolder || peek().type == TokenType::True ||
          peek().type == TokenType::False || peek().type == TokenType::Identifier ||
          peek().type == TokenType::Null) {
        
        // Semantic check: NULL cannot be compared with =, !=, <, >, <=, >=
        if (peek().type == TokenType::Null && 
            (op == TokenType::Eq || op == TokenType::Ne || op == TokenType::Lt ||
             op == TokenType::Gt || op == TokenType::Le || op == TokenType::Ge)) {
          result.has_error = true;
          result.error = SQLErrorKind::NullComparisonNotAllowed;
          result.err_idx = peek().pos;
          return;
        }
        
        advance();
        
        // Skip any dot notation (table.column)
        while (peek().type == TokenType::Dot) {
          advance();
          if (peek().type == TokenType::Identifier) {
            advance();
          }
        }
      }
      continue;
    }
    
    // For arithmetic operators, just skip for now
    if (is_arithmetic_operator(peek().type)) {
      advance();
      continue;
    }
    
    // Otherwise, consume the token
    advance();
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_and_expr(ParseResult &result) {
  result.has_left_column = false;
  parse_primary_expr(result);

  while (peek().type == TokenType::And) {
    advance();
    result.has_left_column = false;
    parse_primary_expr(result);
  }
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_where_clause(ParseResult &result) {
  parse_and_expr(result);
  if (result.has_error)
    return;

  while (peek().type == TokenType::Or) {
    advance();
    parse_and_expr(result);
  }
}

} // namespace ess::orm::sql