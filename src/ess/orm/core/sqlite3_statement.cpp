#include "sqlite3_statement.h"
#include <format>
#include <memory_resource>
#include <source_location>

namespace ess::orm::core::sqlite3_impl {

Sqlite3Statemnet::Sqlite3Statemnet(sqlite3 *db, std::string_view sql)
    : Statement(), m_db_ref(db) {
  sqlite3_stmt *raw = nullptr;
  if (sqlite3_prepare_v2(m_db_ref, sql.data(), sql.size(), &raw, nullptr) !=
      SQLITE_OK) {
    std::source_location loc;
    throw std::runtime_error(std::string("[Orm Error] In []: ") +
                             sqlite3_errmsg(db));
  }
  m_stmt.reset(raw);
}

bool Sqlite3Statemnet::next() {
  int rc = sqlite3_step(m_stmt.get());
  if (rc == SQLITE_ROW)
    return true;
  if (rc == SQLITE_DONE)
    return false;
  throw std::runtime_error(std::string("[Orm Error]: ") +
                           sqlite3_errmsg(m_db_ref));
}

void Sqlite3Statemnet::reset() {
  if (sqlite3_reset(m_stmt.get()) != SQLITE_OK) {
    throw std::runtime_error(std::string("[Orm Error] Reset: ") +
                             sqlite3_errmsg(m_db_ref));
  }
};

void Sqlite3Statemnet::clear_bindings() {
  if (sqlite3_clear_bindings(m_stmt.get()) != SQLITE_OK) {
    throw std::runtime_error(std::string("[Orm Error] ClearBindings: ") +
                             sqlite3_errmsg(m_db_ref));
  }
};

int Sqlite3Statemnet::column_count() const {};

int Sqlite3Statemnet::column_int(int index) const {};
int64_t Sqlite3Statemnet::column_int64(int index) const {};
double Sqlite3Statemnet::column_double(int index) const {};
std::string Sqlite3Statemnet::column_text(int index) const {};
bool Sqlite3Statemnet::column_is_null(int index) const {};
std::string Sqlite3Statemnet::column_name(int index) const {};

void Sqlite3Statemnet::bind_one(int index, bool param) {
  if (sqlite3_bind_int(m_stmt.get(), index, param ? 1 : 0) != SQLITE_OK) {
    throw std::runtime_error(std::string("[Orm Error] ClearBindings: ") +
                             sqlite3_errmsg(m_db_ref));
  }
};
void Sqlite3Statemnet::bind_one(int index, int param) {};
void Sqlite3Statemnet::bind_one(int index, int64_t param) {};
void Sqlite3Statemnet::bind_one(int index, double param) {};
void Sqlite3Statemnet::bind_one(int index, std::string_view param) {};
void Sqlite3Statemnet::bind_one(int index) {};
} // namespace ess::orm::core::sqlite3_impl
