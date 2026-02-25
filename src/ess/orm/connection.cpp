#include <ess/orm/connection.hpp>
#include <ess/orm/statement.hpp>
#include <sqlite3.h>
#include <std.hpp>

namespace ess::orm {

// Connection 的 prepare_cached 由于使用了 shared_from_this，则 Connection
// 必须被 shared_ptr 管理，直接构造会出错，因此考虑禁用构造函数，使用工厂模式
Connection::Connection(std::string_view connection_url) {
  sqlite3 *raw_db = nullptr;
  if (sqlite3_open_v2(connection_url.data(), &raw_db,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX,
                      nullptr) != SQLITE_OK) {
    throw std::runtime_error(std::string("Can't open the database: ") +
                             connection_url.data() + sqlite3_errmsg(raw_db));
  }
  m_db.reset(raw_db);
  configure();
}

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

void Connection::configure() {
  execute_raw("PRAGMA journal_mode=WAL;");   // WAL 模式
  execute_raw("PRAGMA synchronous=NORMAL;"); // 平衡性能和安全
  execute_raw("PRAGMA cache_size=-64000;");  // 缓存大小 64MB
  execute_raw("PRAGMA temp_store=MEMORY;");  // 临时表存储在内存
  execute_raw("PRAGMA busy_timeout=30000;"); // 等待 30 秒
}

// 提供接口构造 Connection
std::shared_ptr<Connection>
Connection::create(std::string const &connection_url) {
  struct Enabler : Connection {
    Enabler(std::string_view url) : Connection(url) {}
  };
  return std::make_shared<Enabler>(connection_url);
}

void Connection::execute_raw(std::string_view sql) {
  char *err_msg = nullptr;
  if (sqlite3_exec(m_db.get(), sql.data(), nullptr, nullptr, &err_msg) !=
      SQLITE_OK) {
    std::string err = err_msg ? err_msg : sqlite3_errmsg(m_db.get());
    sqlite3_free(err_msg);
    throw std::runtime_error("SQL Error: " + err);
  }
}

} // namespace ess::orm
