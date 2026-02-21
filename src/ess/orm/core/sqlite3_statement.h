#pragma once
#include <ess/orm/core/connection.h>
#include <ess/orm/core/statement.h>
#include <sqlite3.h>

namespace ess::orm::core::sqlite3_impl {

class Sqlite3Statemnet final : public Statement {
  struct Deleter {
    void operator()(sqlite3_stmt *stmt) { sqlite3_finalize(stmt); }
  };
  std::unique_ptr<sqlite3_stmt, Deleter> m_stmt;
  sqlite3 *m_db_ref;

public:
  Sqlite3Statemnet(sqlite3 *db, std::string_view sql);

  Sqlite3Statemnet(Sqlite3Statemnet const &) = delete;
  Sqlite3Statemnet(Sqlite3Statemnet &&) noexcept = default;
  Sqlite3Statemnet &operator=(Sqlite3Statemnet &&) noexcept = default;

  ~Sqlite3Statemnet() = default;

  bool next() override;
  void reset() override;
  void clear_bindings() override;

  int column_count() const override;
  int column_int(int index) const override;
  int64_t column_int64(int index) const override;
  double column_double(int index) const override;
  std::string column_text(int index) const override;
  bool column_is_null(int index) const override;
  std::string column_name(int index) const override;

private:
  void bind_one(int index, bool param) override;
  void bind_one(int index, int param) override;
  void bind_one(int index, int64_t param) override;
  void bind_one(int index, double param) override;
  void bind_one(int index, std::string_view param) override;
  void bind_one(int index) override;
};

} // namespace ess::orm::core::sqlite3_impl
