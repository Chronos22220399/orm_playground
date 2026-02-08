// ast/select.hpp
#pragma once
#include <ess/orm/sql/ast/common.hpp>
#include <ess/orm/sql/ast/storage.hpp>

namespace ess::orm::sql::ast {

// ========== 表引用 ==========
struct SelectTableRef {
  enum class Kind : uint8_t {
    Table,
    Subquery,
    Join,
  };

  Kind kind = Kind::Table;

  // 普通表
  QualifiedName table_name;
  Identifier alias;

  // 子查询引用
  StmtRef subquery_ref;

  // JOIN 信息
  struct JoinInfo {
    TokenType join_type = TokenType::Join;
    ExprRef on_condition;
    bool natural = false;
  } join;

  constexpr bool has_alias() const { return !alias.empty(); }
};

// ========== SELECT 语句 ==========
struct SelectStmt {
  bool distinct = false;
  bool all = false;

  // 结果列
  std::array<ResultColumn, AstConfig::MaxColumns> columns{};
  std::size_t column_count = 0;

  // FROM
  std::array<SelectTableRef, AstConfig::MaxTables> from_tables{};
  std::size_t from_count = 0;

  // WHERE
  bool has_where = false;
  ExprRef where_expr;

  // GROUP BY
  std::array<ExprRef, AstConfig::MaxGroupBy> group_bys{};
  std::size_t group_by_count = 0;

  // HAVING
  bool has_having = false;
  ExprRef having_expr;

  // UNION / INTERSECT / EXCEPT
  enum class CompoundOp : uint8_t {
    None,
    Union,
    UnionAll,
    Intersect,
    Except,
  };
  CompoundOp compound_op = CompoundOp::None;
  StmtRef compound_stmt_ref;

  // ORDER BY
  std::array<OrderByItem, AstConfig::MaxOrderBy> order_bys{};
  std::size_t order_by_count = 0;

  // LIMIT / OFFSET
  bool has_limit = false;
  ExprRef limit_expr;
  bool has_offset = false;
  ExprRef offset_expr;
};

} // namespace ess::orm::sql::ast
