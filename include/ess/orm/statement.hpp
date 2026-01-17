#include <ess/orm/dsl.hpp>
#include <ess/orm/result_set_mapper.hpp>
#include <ess/orm/sql_destroier.hpp>
#include <sqlite3.h>

namespace ess::orm {

class Statement {
  StatementPtr m_stmt;

public:
  Statement() = default;

  Statement(sqlite3 *db, std::string_view sql) { prepare(db, sql); }

  Statement(Statement const &) = delete;

  Statement(Statement &&other) noexcept = default;

  Statement &operator=(Statement &&other) noexcept = default;

  sqlite3_stmt *get() const { return m_stmt.get(); }

  void prepare(sqlite3 *db, std::string_view sql) {
    sqlite3_stmt *raw = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.data(), static_cast<int>(sql.length()),
                                &(raw), nullptr);
    if (rc != SQLITE_OK) {
      std::string err_msg = "Sqlite Prepare Error: ";
      err_msg += sqlite3_errmsg(db);
      err_msg += "\nSQL: ";
      err_msg += sql;
      throw std::runtime_error(sqlite3_errmsg(db));
    }
    m_stmt.reset(raw);
  }

  void bind_params(auto &&...args) {
    constexpr auto count = sizeof...(args);
    [&]<size_t... I>(std::index_sequence<I...>) {
      (bind_one(I + 1, args), ...);
    }(std::make_index_sequence<count>{});

    auto expanded = expanded_sql();
    fmt::println("SQL: {}", expanded);
  }

  std::string expanded_sql() {
    ExpanedSqlPtr expanded(sqlite3_expanded_sql(m_stmt.get()));

    if (!expanded)
      return "";
    return std::string(expanded.get());
  }

  void reset() {
    if (sqlite3_reset(m_stmt.get()) != SQLITE_OK) {
      throw std::runtime_error(sqlite3_errmsg(sqlite3_db_handle(m_stmt.get())));
    }
  }

  void clear_bindings() {
    if (sqlite3_clear_bindings(m_stmt.get()) != SQLITE_OK) {
      throw std::runtime_error(sqlite3_errmsg(sqlite3_db_handle(m_stmt.get())));
    }
  }

  bool next() {
    if (sqlite3_step(m_stmt.get()) != SQLITE_ROW) {
      return false;
    }
    return true;
  }

private:
  void bind_one(int index, int param) {
    sqlite3_bind_int(m_stmt.get(), index, param);
  }

  void bind_one(int index, double param) {
    sqlite3_bind_double(m_stmt.get(), index, param);
  }

  void bind_one(int index, std::string const &param) {
    sqlite3_bind_text(m_stmt.get(), index, param.c_str(), -1, SQLITE_TRANSIENT);
  }

  void bind_one(int index, std::string_view param) {
    sqlite3_bind_text(m_stmt.get(), index, param.data(),
                      static_cast<int>(param.length()), SQLITE_TRANSIENT);
  }

  void bind_one(int index) { sqlite3_bind_null(m_stmt.get(), index); }
};
} // namespace ess::orm
