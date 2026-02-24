#pragma once
#include <ess/orm/core/conn_factory.h>
#include <ess/orm/core/connection.h>
#include <sqlite3.h>
#include <string_view>
#include <unordered_map>

namespace ess::orm::core::sqlite3_impl {

class ESS_ORM_API Sqlite3Connection final : public Connection {
protected:
  friend struct ConnFactory<dialect::Sqlite3>;

  explicit Sqlite3Connection(std::string_view connection_url);

  void configure();

  struct Deleter {
    void operator()(sqlite3 *db) const {
      if (db)
        sqlite3_close(db);
    }
  };

public:
  [[nodiscard]]
  StatementPtr prepare(std::string_view sql) override;
  [[nodiscard]]
  Statement &prepare_cached(std::string_view sql) override;

  void begin_transaction() override;
  void commit() override;
  void rollback() override;

  bool is_open() override;

private:
  std::unique_ptr<sqlite3, Deleter> m_db = nullptr;
  std::unordered_map<std::string, StatementPtr> m_stmt_cache{};
};

} // namespace ess::orm::core::sqlite3_impl
