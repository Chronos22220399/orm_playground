#pragma once
#include "../parser_decl.hpp"

namespace ess::orm::sql {

template <std::size_t TokenCount>
constexpr ParseResult Parser<TokenCount>::parse_select(bool allow_rparen_end) {
  ParseResult result{};
  if (peek().type == TokenType::Select) {
    advance();
  } else {
    result.has_error = true;
    result.error = SQLErrorKind::UnknownBeginning;
    result.err_idx = peek().pos;
    return result;
  }

  if (peek().type == TokenType::Distinct || peek().type == TokenType::All) {
    advance();
  }

  // Parse SELECT list (columns, expressions, aggregates, etc.)
  parse_columns(result);
  if (result.has_error)
    return result;
  if (result.column_count == 0 && !result.is_star) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedStarOrColumns;
    result.err_idx = peek().pos;
    return result;
  }

  if (!match(TokenType::From, result, SQLErrorKind::ExpectedFrom)) {
    return result;
  }

  if (peek().type == TokenType::Identifier) {
    result.table.name_pos = peek().pos;
    result.table.name_len = peek().len;
    advance();

    if (peek().type == TokenType::Identifier) {
      result.table.alias.pos = peek().pos;
      result.table.alias.len = peek().len;
      advance();
    }
  } else if (peek().type == TokenType::Lparen) {
    result.table.is_subquery = true;
    while (peek().type != TokenType::Rparen && peek().type != TokenType::End) {
      advance();
    }
    if (peek().type == TokenType::Rparen) {
      advance();
    }
    if (peek().type == TokenType::Identifier) {
      result.table.alias.pos = peek().pos;
      result.table.alias.len = peek().len;
      advance();
    }
  } else {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedTableAfterFrom;
    result.err_idx = peek().pos;
    return result;
  }

  while (true) {
    if (peek().type == TokenType::Comma) {
      advance();
      if (result.join_count >= result.joins.size()) {
        result.has_error = true;
        result.error = SQLErrorKind::UnknownError;
        result.err_idx = peek().pos;
        return result;
      }

      auto &join = result.joins[result.join_count];
      if (peek().type == TokenType::Identifier) {
        join.name_pos = peek().pos;
        join.name_len = peek().len;
        advance();

        if (peek().type == TokenType::Identifier) {
          join.alias.pos = peek().pos;
          join.alias.len = peek().len;
          advance();
        }
        result.join_count++;
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedTableAfterFrom;
        return result;
      }
    } else if (peek().type == TokenType::Join ||
               peek().type == TokenType::Left ||
               peek().type == TokenType::Right ||
               peek().type == TokenType::Inner ||
               peek().type == TokenType::Outer ||
               peek().type == TokenType::Cross) {

      if (result.join_count >= result.joins.size()) {
        result.has_error = true;
        result.error = SQLErrorKind::UnknownError;
        result.err_idx = peek().pos;
        return result;
      }

      auto &join = result.joins[result.join_count];
      join.join_type = peek().type;

      if (peek().type == TokenType::Left || peek().type == TokenType::Right ||
          peek().type == TokenType::Inner || peek().type == TokenType::Outer ||
          peek().type == TokenType::Cross) {
        advance();
      }

      if (peek().type == TokenType::Join) {
        advance();
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedTableAfterFrom;
        return result;
      }

      if (peek().type == TokenType::Identifier) {
        join.name_pos = peek().pos;
        join.name_len = peek().len;
        advance();

        if (peek().type == TokenType::Identifier) {
          join.alias.pos = peek().pos;
          join.alias.len = peek().len;
          advance();
        }
      } else if (peek().type == TokenType::Lparen) {
        join.is_subquery = true;
        auto res = parse_select(true);
        if (res.has_error) {
          result.has_error = true;
          result.error = res.error;
          result.err_idx = res.err_idx;
          return result;
        }
        if (peek().type == TokenType::Identifier) {
          join.alias.pos = peek().pos;
          join.alias.len = peek().len;
          advance();
        }
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedTableAfterFrom;
        return result;
      }

      if (peek().type == TokenType::On) {
        advance();
        join.on_pos = peek().pos;
        while (peek().type != TokenType::And && peek().type != TokenType::Or &&
               peek().type != TokenType::End &&
               peek().type != TokenType::Group &&
               peek().type != TokenType::Order &&
               peek().type != TokenType::Limit &&
               peek().type != TokenType::Rparen && !allow_rparen_end) {
          advance();
        }
        join.on_len = peek().pos - join.on_pos;
      }

      result.join_count++;
    } else {
      break;
    }
  }

  if (peek().type == TokenType::Where) {
    result.has_left_column = false;
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
      result.err_idx = peek().pos;
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

  if (peek().type == TokenType::Limit) {
    advance();
    if (peek().type == TokenType::Number ||
        peek().type == TokenType::PlaceHolder) {
      result.limit_pos = peek().pos;
      result.limit_len = peek().len;
      // Record placeholder for semantic analysis
      if (peek().type == TokenType::PlaceHolder) {
        result.semantic_result.add_placeholder(peek().pos);
      }
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedTableAfterFrom;
      result.err_idx = peek().pos;
      return result;
    }

    if (peek().type == TokenType::Offset) {
      advance();
      if (peek().type == TokenType::Number ||
          peek().type == TokenType::PlaceHolder) {
        result.offset_pos = peek().pos;
        result.offset_len = peek().len;
        // Record placeholder for semantic analysis
        if (peek().type == TokenType::PlaceHolder) {
          result.semantic_result.add_placeholder(peek().pos);
        }
        advance();
      } else {
        result.has_error = true;
        result.error = SQLErrorKind::ExpectedTableAfterFrom;
        result.err_idx = peek().pos;
        return result;
      }
    }
  }

  if (at_end()) {
    return result;
  }

  if (allow_rparen_end && peek().type == TokenType::Rparen) {
    return result;
  }

  // If the next token is a compound operator (UNION, INTERSECT, EXCEPT),
  // return without error - the caller (parse_compound_select) will handle it
  if (peek().type == TokenType::Union ||
      peek().type == TokenType::Intersect ||
      peek().type == TokenType::Except) {
    return result;
  }

  result.has_error = true;
  result.error = SQLErrorKind::InvalidEnd;
  result.err_idx = peek().pos;
  return result;
}

template <std::size_t TokenCount>
constexpr void Parser<TokenCount>::parse_with_clause(ParseResult &result) {
  // WITH [RECURSIVE] cte_name [ (column_list) ] AS (subquery) [, ...]
  // Debug: force error to see if function is reached
  // result.has_error = true;
  // result.error = SQLErrorKind::UnknownError;
  // return;
  advance(); // consume WITH

  // Optional RECURSIVE
  if (peek().type == TokenType::Recursive) {
    advance();
  }

  // Parse one or more CTE definitions
  while (true) {
    // CTE name
    if (peek().type != TokenType::Identifier) {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedIdentifier;
      result.err_idx = peek().pos;
      return;
    }
    advance();

    // Optional column list
    if (peek().type == TokenType::Lparen) {
      advance();
      // Skip column list until matching right parenthesis
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

    // AS keyword
    if (!match(TokenType::As, result, SQLErrorKind::ExpectedIdentifier)) {
      return;
    }

    // Left parenthesis before subquery
    if (!match(TokenType::Lparen, result,
               SQLErrorKind::ExpectedLeftParenAfterIn)) {
      return;
    }

    // Parse subquery (SELECT statement)
    auto subquery_result = parse_select(true);
    if (subquery_result.has_error) {
      result.has_error = true;
      result.error = subquery_result.error;
      result.err_idx = subquery_result.err_idx;
      return;
    }

    // Consume the right parenthesis that closes the subquery
    if (peek().type == TokenType::Rparen) {
      advance();
    } else {
      result.has_error = true;
      result.error = SQLErrorKind::ExpectedRightParen;
      result.err_idx = peek().pos;
      return;
    }

    // Check for comma to continue with next CTE definition
    if (peek().type == TokenType::Comma) {
      advance();
      continue;
    } else {
      break;
    }
  }
  
  // After parsing all CTE definitions, the next token should be SELECT
  // (or another WITH for nested WITH clauses, but we don't support that)
  // Note: This check might be too strict for some valid SQL constructs
  // if (peek().type != TokenType::Select) {
  //   result.has_error = true;
  //   result.error = SQLErrorKind::UnknownBeginning;
  //   result.err_idx = peek().pos;
  //   return;
  // }
}

template <std::size_t TokenCount>
constexpr ParseResult
Parser<TokenCount>::parse_compound_select(bool allow_rparen_end) {
  ParseResult result;

  // Parse first SELECT statement
  auto first_result = parse_select(allow_rparen_end);
  if (first_result.has_error) {
    return first_result;
  }

  // Copy relevant fields from first result
  result = first_result;

  // Check for compound operators
  while (peek().type == TokenType::Union ||
         peek().type == TokenType::Intersect ||
         peek().type == TokenType::Except) {
    advance();

    // Optional ALL after UNION
    if (peek().type == TokenType::All) {
      advance();
    }

    // Parse next SELECT statement
    auto next_result = parse_select(allow_rparen_end);
    if (next_result.has_error) {
      result.has_error = true;
      result.error = next_result.error;
      result.err_idx = next_result.err_idx;
      return result;
    }

    // For compound queries, we need to ensure column counts match (semantic
    // check) For now, just continue parsing if there are more compound
    // operators
  }

  // If we have a compound query, the ORDER BY / LIMIT applies to the entire
  // compound query Those clauses will be parsed by the outer parse_select call
  // (since we pass allow_rparen_end) So we need to handle them here? Actually,
  // the compound query is parsed as part of a larger SELECT parsing, so the
  // ORDER BY / LIMIT parsing will happen after this function returns. For
  // simplicity, we'll let the outer parser handle those clauses.

  return result;
}

template <std::size_t TokenCount>
[[nodiscard]] constexpr ParseResult Parser<TokenCount>::parse() {
  // Check for WITH clause
  if (peek().type == TokenType::With) {
    ParseResult result;
    parse_with_clause(result);
    if (result.has_error) {
      return result;
    }
    // After WITH clause, expect a SELECT statement (could be compound)
    // For now, just parse a compound SELECT
    return parse_compound_select(false);
  }

  // Check for INSERT
  if (peek().type == TokenType::Insert) {
    return parse_insert();
  }

  // Check for UPDATE
  if (peek().type == TokenType::Update) {
    return parse_update();
  }

  // Check for DELETE
  if (peek().type == TokenType::Delete) {
    return parse_delete();
  }

  // Otherwise, parse a SELECT statement (could be compound)
  if (peek().type == TokenType::Select) {
    return parse_compound_select(false);
  }

  return ParseResult::make_error();
}

} // namespace ess::orm::sql
