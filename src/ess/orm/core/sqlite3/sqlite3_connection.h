#pragma once
#include <ess/orm/core/conn_factory.hpp>
#include <ess/orm/core/connection.hpp>
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
  StatementPtr prepare(std::string_view sql) override;
  Statement &prepare_cached(std::string_view sql) override;

  void begin_transaction(TxMode mode) override;
  void commit() override;
  void rollback() override;
  int nesting_level() const noexcept override;

  bool is_open() override;

  int64_t last_insert_id() override;
  int64_t affected_rows() override;

private:
  std::unique_ptr<sqlite3, Deleter> m_db = nullptr;
  std::unordered_map<std::string, StatementPtr> m_stmt_cache{};
  int m_nesting_level = 0;
};

} // namespace ess::orm::core::sqlite3_impl
