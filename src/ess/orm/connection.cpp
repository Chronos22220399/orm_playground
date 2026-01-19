#include <std.hpp>
#include <ess/orm/connection.h>
#include <ess/orm/statement.h>

namespace ess::orm {

Statement &Connection::prepare_cached(std::string_view sql) {
  std::string sql_str(sql);
  if (auto it = m_stmt_cache.find(sql_str); it != m_stmt_cache.end()) {
    it->second->reset();
    it->second->clear_bindings();
    return *(it->second);
  }
  auto stmt_ptr = std::make_unique<Statement>(shared_from_this(), sql);
  auto [it, success] =
      m_stmt_cache.emplace(std::move(sql_str), std::move(stmt_ptr));
  return *(it->second);
}
} // namespace ess::orm
