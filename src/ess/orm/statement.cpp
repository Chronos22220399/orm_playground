#include <ess/orm/connection.hpp>
#include <ess/orm/statement.hpp>
#include <sqlite3.h>
#include <string>

namespace ess::orm {

Statement::StatementGuard::~StatementGuard() {
  m_stmt.reset();
  m_stmt.clear_bindings();
}

Statement::Statement(std::shared_ptr<Connection> conn, std::string_view sql)
    : m_conn_ref(conn) {
  if (auto _conn = m_conn_ref.lock()) {
    prepare(_conn->handle(), sql);
    return;
  }
  throw std::runtime_error("Database connection lost");
}

Statement::~Statement() = default;

sqlite3 *Statement::get_db_handle() const {
  if (auto conn = m_conn_ref.lock()) {
    return conn->handle();
  }
  throw std::runtime_error("Database connection lost");
}

void Statement::prepare(sqlite3 *db, std::string_view sql) {
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

std::string Statement::expanded_sql() {
  ExpanedSqlPtr expanded(sqlite3_expanded_sql(m_stmt.get()));

  if (!expanded)
    return "";
  return std::string(expanded.get());
}

void Statement::reset() {
  if (sqlite3_reset(m_stmt.get()) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(sqlite3_db_handle(m_stmt.get())));
  }
}

void Statement::clear_bindings() {
  if (sqlite3_clear_bindings(m_stmt.get()) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(sqlite3_db_handle(m_stmt.get())));
  }
}

bool Statement::next() {
  if (sqlite3_step(m_stmt.get()) != SQLITE_ROW) {
    return false;
  }
  return true;
}

void Statement::bind_one(int index, bool param) {
  sqlite3_bind_int(m_stmt.get(), index, param ? 1 : 0);
}

void Statement::bind_one(int index, int param) {
  sqlite3_bind_int(m_stmt.get(), index, param);
}

void Statement::bind_one(int index, int64_t param) {
  sqlite3_bind_int64(m_stmt.get(), index, param);
}

void Statement::bind_one(int index, double param) {
  sqlite3_bind_double(m_stmt.get(), index, param);
}

void Statement::bind_one(int index, std::string_view param) {
  sqlite3_bind_text(m_stmt.get(), index, param.data(),
                    static_cast<int>(param.length()), SQLITE_TRANSIENT);
}

void Statement::bind_one(int index, std::string const &param) {
  sqlite3_bind_text(m_stmt.get(), index, param.c_str(), -1, SQLITE_TRANSIENT);
}

void Statement::bind_one(int index, const char *param) {
  sqlite3_bind_text(m_stmt.get(), index, param, -1, SQLITE_TRANSIENT);
}

void Statement::bind_one(int index) { sqlite3_bind_null(m_stmt.get(), index); }

} // namespace ess::orm
