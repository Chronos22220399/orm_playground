#pragma once
#include <ess/orm/defines.h>
#include <ess/orm/sql_destroier.h>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ess::orm {
class ESS_ORM_API Statement;
class ESS_ORM_API ConnectionPool;

class ESS_ORM_API Connection : public std::enable_shared_from_this<Connection> {
  friend class ConnectionPool;

  using StatementPtr = std::unique_ptr<Statement>;
  using SqlitePtr = std::unique_ptr<sqlite3, SqliteDestroier>;

  SqlitePtr m_db = nullptr;
  std::unordered_map<std::string, StatementPtr> m_stmt_cache{};

  explicit Connection(std::string_view connection_url);

  // TODO: 后续将 configure 拆开，由配置结果来选择开启的模式
  void configure();

public:
  static std::shared_ptr<Connection> create(std::string const &connection_url);

  Connection(Connection const &) = delete;

  // 使用了 shared_from_this，因此禁用
  Connection(Connection &&) = delete;

  Connection &operator=(Connection &&) = delete;

  Statement &prepare_cached(std::string_view sql);

  sqlite3 *handle() { return m_db.get(); }

  void execute_raw(std::string_view sql);
};

} // namespace ess::orm
