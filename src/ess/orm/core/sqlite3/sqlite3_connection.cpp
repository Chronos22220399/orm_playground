#include "sqlite3_connection.h"
#include "sqlite3_statement.h"
#include <ess/orm/common/error.hpp>
#include <ess/orm/core/connection.hpp>
#include <format>
#include <sqlite3.h>

namespace ess::orm::core::sqlite3_impl {

Sqlite3Connection::Sqlite3Connection(std::string_view connection_url)
    : Connection() {
  sqlite3 *raw_db = nullptr;
  if (sqlite3_open_v2(connection_url.data(), &raw_db,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX,
                      nullptr) != SQLITE_OK) {
    throw std::runtime_error(
        get_cur_loc_info() + std::string(": Can't open the database: ") +
        connection_url.data() + ": " + sqlite3_errmsg(raw_db));
  }
  m_db.reset(raw_db);
  configure();
}

void Sqlite3Connection::configure() {
  execute_raw("PRAGMA journal_mode=WAL;");   // WAL 模式
  execute_raw("PRAGMA synchronous=NORMAL;"); // 平衡性能和安全
  execute_raw("PRAGMA cache_size=-64000;");  // 缓存大小 64MB
  execute_raw("PRAGMA temp_store=MEMORY;");  // 临时表存储在内存
  execute_raw("PRAGMA busy_timeout=30000;"); // 等待 30 秒
}

StatementPtr Sqlite3Connection::prepare(std::string_view sql) {
  return std::make_unique<Sqlite3Statemnet>(m_db.get(), sql);
}

Statement &Sqlite3Connection::prepare_cached(std::string_view sql) {
  std::string sql_str(sql);
  if (auto it = m_stmt_cache.find(sql_str); it != m_stmt_cache.end()) {
    it->second->reset();
    it->second->clear_bindings();
    return *(it->second);
  }
  std::unique_ptr<Statement> stmt_ptr =
      std::make_unique<Sqlite3Statemnet>(m_db.get(), sql);
  auto [it, success] =
      m_stmt_cache.emplace(std::move(sql_str), std::move(stmt_ptr));
  return *(it->second);
}

void Sqlite3Connection::begin_transaction(TxMode mode) {
  if (m_nesting_level == 0) {
    if (mode == TxMode::WRITE) {
      execute_raw("BEGIN IMMEDIATE");
    } else {
      execute_raw("BEGIN DEFERRED");
    }
  } else {
    execute_raw(std::format("SAVEPOINT sp_{}", m_nesting_level));
  }
  ++m_nesting_level;
};

void Sqlite3Connection::commit() {
  --m_nesting_level;
  if (m_nesting_level == 0) {
    execute_raw("COMMIT");
  } else {
    execute_raw(std::format("RELEASE SAVEPOINT sp_{}", m_nesting_level));
  }
};

void Sqlite3Connection::rollback() {
  --m_nesting_level;
  if (m_nesting_level == 0) {
    execute_raw("ROLLBACK");
  } else {
    execute_raw(std::format("ROLLBACK TO SAVEPOINT sp_{}", m_nesting_level));
  }
};

int Sqlite3Connection::nesting_level() const noexcept {
  return m_nesting_level;
}

bool Sqlite3Connection::is_open() { return m_db != nullptr; };

int64_t Sqlite3Connection::last_insert_id() {
  return static_cast<int64_t>(sqlite3_last_insert_rowid(m_db.get()));
}

int64_t Sqlite3Connection::affected_rows() {
  return static_cast<int64_t>(sqlite3_changes64(m_db.get()));
}
} // namespace ess::orm::core::sqlite3_impl
