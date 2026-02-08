// ast/storage.hpp
#pragma once
#include <ess/orm/sql/ast/common.hpp>

namespace ess::orm::sql::ast {

// ========== 语句类型 ==========
enum class StmtKind : uint8_t {
  Select,
  Insert,
  Update,
  Delete,
};

// ========== 冲突处理 ==========
enum class ConflictAction : uint8_t {
  None,
  Abort,
  Rollback,
  Fail,
  Ignore,
  Replace,
};

} // namespace ess::orm::sql::ast
