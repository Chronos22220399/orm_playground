// ast/delete.hpp
#pragma once
#include <ess/orm/sql/ast/common.hpp>
#include <ess/orm/sql/ast/storage.hpp>

namespace ess::orm::sql::ast {

// ========== DELETE 语句 ==========
struct DeleteStmt {
  QualifiedName table_name;
  Identifier alias;

  // WHERE
  bool has_where = false;
  ExprRef where_expr;

  // ORDER BY
  std::array<OrderByItem, AstConfig::MaxOrderBy> order_bys{};
  std::size_t order_by_count = 0;

  // LIMIT
  bool has_limit = false;
  ExprRef limit_expr;

  // RETURNING
  std::array<ResultColumn, AstConfig::MaxColumns> returning{};
  std::size_t returning_count = 0;
};

} // namespace ess::orm::sql::ast
