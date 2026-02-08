// ast/update.hpp
#pragma once
#include <ess/orm/sql/ast/common.hpp>
#include <ess/orm/sql/ast/select.hpp>
#include <ess/orm/sql/ast/storage.hpp>

namespace ess::orm::sql::ast {

// ========== SET 项 ==========
struct UpdateSetItem {
  QualifiedName column;
  ExprRef value;
};

// ========== UPDATE 语句 ==========
struct UpdateStmt {
  ConflictAction conflict_action = ConflictAction::None;

  QualifiedName table_name;
  Identifier alias;

  // SET
  std::array<UpdateSetItem, AstConfig::MaxSetItems> sets{};
  std::size_t set_count = 0;

  // FROM（SQLite 3.33+）
  std::array<SelectTableRef, AstConfig::MaxTables> from_tables{};
  std::size_t from_count = 0;

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
