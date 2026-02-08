// ast/statement.hpp
#pragma once
#include <ess/orm/sql/ast/delete.hpp>
#include <ess/orm/sql/ast/insert.hpp>
#include <ess/orm/sql/ast/select.hpp>
#include <ess/orm/sql/ast/update.hpp>

namespace ess::orm::sql::ast {

// ========== 语句节点（平铺存储，编译期内存不是问题） ==========
struct StmtNode {
  StmtKind kind = StmtKind::Select;

  // 平铺存储所有类型
  SelectStmt select{};
  InsertStmt insert{};
  UpdateStmt update{};
  DeleteStmt delete_{};

  constexpr StmtNode() = default;

  // 类型检查
  constexpr bool is_select() const { return kind == StmtKind::Select; }
  constexpr bool is_insert() const { return kind == StmtKind::Insert; }
  constexpr bool is_update() const { return kind == StmtKind::Update; }
  constexpr bool is_delete() const { return kind == StmtKind::Delete; }

  // 获取数据
  constexpr SelectStmt &as_select() { return select; }
  constexpr InsertStmt &as_insert() { return insert; }
  constexpr UpdateStmt &as_update() { return update; }
  constexpr DeleteStmt &as_delete() { return delete_; }

  constexpr const SelectStmt &as_select() const { return select; }
  constexpr const InsertStmt &as_insert() const { return insert; }
  constexpr const UpdateStmt &as_update() const { return update; }
  constexpr const DeleteStmt &as_delete() const { return delete_; }

  // 初始化（只设置 kind，结构体已默认初始化）
  constexpr void init_select() { kind = StmtKind::Select; }
  constexpr void init_insert() { kind = StmtKind::Insert; }
  constexpr void init_update() { kind = StmtKind::Update; }
  constexpr void init_delete() { kind = StmtKind::Delete; }
};

// ========== 解析上下文 ==========
struct ParseContext {
  std::array<StmtNode, AstConfig::MaxSubqueries> stmts{};
  std::size_t stmt_count = 0;

  ExprPool<AstConfig::MaxExprs> expr_pool{};

  constexpr StmtRef alloc_stmt() {
    if (stmt_count >= AstConfig::MaxSubqueries) {
      return StmtRef::invalid();
    }
    return StmtRef::make(stmt_count++);
  }

  constexpr ExprRef alloc_expr() { return expr_pool.alloc(); }

  constexpr StmtNode &stmt(StmtRef ref) { return stmts[ref.index]; }
  constexpr const StmtNode &stmt(StmtRef ref) const { return stmts[ref.index]; }

  constexpr StmtNode &operator[](StmtRef ref) { return stmts[ref.index]; }
  constexpr const StmtNode &operator[](StmtRef ref) const {
    return stmts[ref.index];
  }

  constexpr Expr &expr(ExprRef ref) { return expr_pool[ref]; }
  constexpr const Expr &expr(ExprRef ref) const { return expr_pool[ref]; }

  constexpr StmtNode &root() { return stmts[0]; }
  constexpr const StmtNode &root() const { return stmts[0]; }
};

} // namespace ess::orm::sql::ast
