#pragma once
#include "../parser_decl.hpp"

namespace ess::orm::sql {

template <std::size_t TokenCount>
constexpr ParseResult Parser<TokenCount>::parse_delete() {
  ParseResult result;
  result.is_delete = true;

  // DELETE
  if (!match(TokenType::Delete, result, SQLErrorKind::ExpectedDelete)) {
    return result;
  }

  // FROM
  if (!match(TokenType::From, result, SQLErrorKind::ExpectedFromAfterDelete)) {
    return result;
  }

  // table_name
  if (peek().type != TokenType::Identifier) {
    result.has_error = true;
    result.error = SQLErrorKind::ExpectedTableAfterFrom;
    result.err_idx = peek().pos;
    return result;
  }
  result.delete_stmt.table_name_pos = peek().pos;
  result.delete_stmt.table_name_len = peek().len;
  advance();

  // Optional WHERE clause
  if (peek().type == TokenType::Where) {
    advance();
    result.delete_stmt.where_pos = peek().pos;
    
    // Parse WHERE expression
    parse_where_clause(result);
    if (result.has_error) {
      return result;
    }
    
    // Calculate WHERE expression length
    result.delete_stmt.where_len = previous().pos + previous().len - result.delete_stmt.where_pos;
  }

  return result;
}

} // namespace ess::orm::sql