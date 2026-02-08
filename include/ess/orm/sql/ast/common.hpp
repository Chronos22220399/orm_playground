// ast/common.hpp
#pragma once
#include <array>
#include <cstdint>
#include <ess/orm/meta.hpp>
#include <ess/orm/sql/token.hpp>

namespace ess::orm::sql::ast {

// 最大容量配置
struct AstConfig {
  static constexpr std::size_t MaxExprs = 128;
  static constexpr std::size_t MaxColumns = 32;
  static constexpr std::size_t MaxTables = 8;
  static constexpr std::size_t MaxSubqueries = 16;
  static constexpr std::size_t MaxOrderBy = 8;
  static constexpr std::size_t MaxGroupBy = 8;
  static constexpr std::size_t MaxValues = 64;
  static constexpr std::size_t MaxSetItems = 32;
};

// 源码位置
struct SourceLocation {
  std::size_t pos = 0;
  std::size_t len = 0;
  std::size_t line = 1;
  std::size_t col = 1;

  constexpr SourceLocation() = default;
  explicit constexpr SourceLocation(const Token &token)
      : pos(token.pos), len(token.len), line(token.line), col(token.col) {}
};

// 标识符
struct Identifier {
  std::size_t pos = 0;
  std::size_t len = 0;

  constexpr bool empty() const { return len == 0; }

  template <std::size_t N>
  constexpr std::string_view text(const meta::FixedString<N> &src) const {
    return std::string_view(src.data() + pos, len);
  }
};

// 限定标识符
struct QualifiedName {
  Identifier schema;
  Identifier table;
  Identifier name;

  constexpr bool has_schema() const { return !schema.empty(); }
  constexpr bool has_table() const { return !table.empty(); }
};

// 表达式索引
struct ExprRef {
  std::size_t index = 0;
  bool valid = false;

  constexpr operator bool() const { return valid; }

  static constexpr ExprRef invalid() { return {0, false}; }
  static constexpr ExprRef make(std::size_t idx) { return {idx, true}; }
};

// 语句索引
struct StmtRef {
  std::size_t index = 0;
  bool valid = false;

  constexpr operator bool() const { return valid; }

  static constexpr StmtRef invalid() { return {0, false}; }
  static constexpr StmtRef make(std::size_t idx) { return {idx, true}; }
};

// 一元操作符
enum class UnaryOp : uint8_t { Minus, Plus, Not, BitNot };

// 二元操作符
enum class BinaryOp : uint8_t {
  Add,
  Sub,
  Mul,
  Div,
  Mod,
  Eq,
  Ne,
  Lt,
  Le,
  Gt,
  Ge,
  And,
  Or,
  BitAnd,
  BitOr,
  BitXor,
  LeftShift,
  RightShift,
  Concat,
  Is,
  IsNot
};

// 表达式节点（平铺存储）
struct Expr {
  enum class Kind : uint8_t {
    Null,
    Literal,
    Column,
    Placeholder,
    Unary,
    Binary,
    Between,
    In,
    Like,
    IsNull,
    Function,
    Case,
    Cast,
    Subquery,
    Exists,
  };

  Kind kind = Kind::Null;
  SourceLocation loc{};

  // Literal
  std::size_t literal_pos = 0;
  std::size_t literal_len = 0;
  bool literal_is_string = false;

  // Column
  QualifiedName column_name{};

  // Placeholder
  std::size_t placeholder_index = 0;

  // Unary
  UnaryOp unary_op = UnaryOp::Minus;
  std::size_t unary_operand_idx = 0;

  // Binary
  BinaryOp binary_op = BinaryOp::Add;
  std::size_t binary_left_idx = 0;
  std::size_t binary_right_idx = 0;

  // Between
  std::size_t between_expr_idx = 0;
  std::size_t between_low_idx = 0;
  std::size_t between_high_idx = 0;
  bool between_is_not = false;

  // In
  std::size_t in_expr_idx = 0;
  bool in_is_subquery = false;
  bool in_is_not = false;
  std::size_t in_subquery_idx = 0;
  std::size_t in_values_start = 0;
  std::size_t in_values_count = 0;

  // Like
  std::size_t like_expr_idx = 0;
  std::size_t like_pattern_idx = 0;
  bool like_is_not = false;

  // IsNull
  std::size_t is_null_expr_idx = 0;
  bool is_null_is_not = false;

  // Function
  Identifier func_name{};
  std::size_t func_args_start = 0;
  std::size_t func_args_count = 0;
  bool func_distinct = false;
  bool func_is_aggregate = false;

  // Subquery / Exists
  std::size_t subquery_stmt_idx = 0;
  bool exists_is_not = false;

  constexpr Expr() = default;

  // 类型检查
  constexpr bool is_null() const { return kind == Kind::Null; }
  constexpr bool is_literal() const { return kind == Kind::Literal; }
  constexpr bool is_column() const { return kind == Kind::Column; }
  constexpr bool is_binary() const { return kind == Kind::Binary; }
  constexpr bool is_function() const { return kind == Kind::Function; }
  constexpr bool is_subquery() const { return kind == Kind::Subquery; }
};

// CASE WHEN 子句
struct CaseWhen {
  ExprRef when_expr;
  ExprRef then_expr;
};

// 排序项
struct OrderByItem {
  ExprRef expr;
  bool desc = false;
  bool nulls_first = false;
  bool nulls_last = false;
};

// 结果列
struct ResultColumn {
  enum class Kind : uint8_t { Star, TableStar, Expr };

  Kind kind = Kind::Expr;
  Identifier table;
  ExprRef expr;
  Identifier alias;
};

// 表达式存储池
template <std::size_t MaxExprs = AstConfig::MaxExprs> struct ExprPool {
  std::array<Expr, MaxExprs> exprs{};
  std::size_t count = 0;

  std::array<std::size_t, MaxExprs> value_indices{};
  std::size_t value_count = 0;

  std::array<CaseWhen, 32> case_whens{};
  std::size_t case_when_count = 0;

  constexpr ExprRef alloc() {
    if (count >= MaxExprs)
      return ExprRef::invalid();
    return ExprRef::make(count++);
  }

  constexpr Expr &operator[](ExprRef ref) { return exprs[ref.index]; }
  constexpr const Expr &operator[](ExprRef ref) const {
    return exprs[ref.index];
  }
};

} // namespace ess::orm::sql::ast
