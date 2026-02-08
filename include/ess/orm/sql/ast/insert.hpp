// ast/insert.hpp
#pragma once
#include <ess/orm/sql/ast/common.hpp>
#include <ess/orm/sql/ast/storage.hpp>

namespace ess::orm::sql::ast {

// ========== 值行 ==========
struct ValueRow {
  std::array<ExprRef, AstConfig::MaxColumns> values{};
  std::size_t count = 0;
};

// ========== UPSERT 子句 ==========
struct UpsertClause {
  std::array<Identifier, 8> conflict_columns{};
  std::size_t conflict_column_count = 0;
  ExprRef conflict_where;

  bool do_nothing = false;

  struct SetItem {
    Identifier column;
    ExprRef value;
  };
  std::array<SetItem, AstConfig::MaxSetItems> updates{};
  std::size_t update_count = 0;
  ExprRef update_where;
};

// ========== INSERT 语句 ==========
struct InsertStmt {
  ConflictAction conflict_action = ConflictAction::None;

  QualifiedName table_name;
  Identifier alias;

  // 列列表
  std::array<Identifier, AstConfig::MaxColumns> columns{};
  std::size_t column_count = 0;

  // 值来源
  enum class ValueSource : uint8_t {
    Values,
    Select,
    DefaultValues,
  };
  ValueSource source = ValueSource::Values;

  // VALUES
  std::array<ValueRow, AstConfig::MaxValues> value_rows{};
  std::size_t row_count = 0;

  // SELECT
  StmtRef select_ref;

  // UPSERT
  bool has_upsert = false;
  UpsertClause upsert;

  // RETURNING
  std::array<ResultColumn, AstConfig::MaxColumns> returning{};
  std::size_t returning_count = 0;
};

} // namespace ess::orm::sql::ast
