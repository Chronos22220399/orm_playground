#pragma once
#include <core_thirdparties.hpp>
#include <ess/orm/sql_destroier.hpp>
#include <unordered_map>

namespace ess::orm {
class Statement;
class ConnectionPool;

class Connection : public std::enable_shared_from_this<Connection> {
  friend class ConnectionPool;

  using StatementPtr = std::unique_ptr<Statement>;

  SqlitePtr m_db = nullptr;
  std::unordered_map<std::string, StatementPtr> m_stmt_cache{};

  // Connection 的 prepare_cached 由于使用了 shared_from_this，则 Connection
  // 必须 被 shared_ptr 管理，直接构造会出错，因此考虑禁用构造函数，使用工厂模式
  explicit Connection(std::string const &connection_url) {
    sqlite3 *raw_db = nullptr;
    if (sqlite3_open_v2(connection_url.c_str(), &raw_db,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX,
                        nullptr) != SQLITE_OK) {
      throw std::runtime_error(std::string("Can't open the database: ") +
                               connection_url.data() + sqlite3_errmsg(raw_db));
    }
    m_db.reset(raw_db);
    configure();
  }

  // TODO: 后续将 configure 拆开，由配置结果来选择开启的模式
  void configure() {
    execute_raw("PRAGMA journal_mode=WAL;");   // WAL 模式
    execute_raw("PRAGMA busy_timeout=30000;"); // 等待 30 秒
    execute_raw("PRAGMA synchronous=NORMAL;"); // 平衡性能和安全
    execute_raw("PRAGMA cache_size=-64000;");  // 缓存大小 64MB
    execute_raw("PRAGMA temp_store=MEMORY;");  // 临时表存储在内存
  }

public:
  static std::shared_ptr<Connection> create(std::string const &connection_url) {
    struct Enabler : Connection {
      Enabler(std::string const &url) : Connection(url) {}
    };
    return std::make_shared<Enabler>(connection_url);
  }

  Connection(Connection const &) = delete;

  // 使用了 shared_from_this，因此禁用
  Connection(Connection &&) = delete;

  Connection &operator=(Connection &&) = delete;

  Statement &prepare_cached(std::string_view sql);

  sqlite3 *handle() { return m_db.get(); }

  void execute_raw(std::string const &sql) {
    char *err_msg = nullptr;
    if (sqlite3_exec(m_db.get(), sql.c_str(), nullptr, nullptr, nullptr) !=
        SQLITE_OK) {
      std::string err(err_msg);
      sqlite3_free(err_msg);
      throw std::runtime_error("SQL Error: " + err);
    }
  }
};

} // namespace ess::orm
