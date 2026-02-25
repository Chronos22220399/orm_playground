#include "sqlite3_statement.h"
#include <ess/orm/error.hpp>

namespace ess::orm::core::sqlite3_impl {

Sqlite3Statemnet::Sqlite3Statemnet(sqlite3 *db, std::string_view sql)
    : Statement(), m_db_ref(db) {
  sqlite3_stmt *raw = nullptr;
  if (sqlite3_prepare_v2(m_db_ref, sql.data(), sql.size(), &raw, nullptr) !=
      SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + ": " +
                             sqlite3_errmsg(m_db_ref) +
                             ": invalid sql: " + sql.data());
  }
  m_stmt.reset(raw);
}

bool Sqlite3Statemnet::next() {
  int rc = sqlite3_step(m_stmt.get());
  if (rc == SQLITE_ROW)
    return true;
  if (rc == SQLITE_DONE)
    return false;
  throw std::runtime_error(get_cur_loc_info() + ": " +
                           sqlite3_errmsg(m_db_ref));
}

void Sqlite3Statemnet::reset() {
  if (sqlite3_reset(m_stmt.get()) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + ": " +
                             sqlite3_errmsg(m_db_ref));
  }
};

void Sqlite3Statemnet::clear_bindings() {
  if (sqlite3_clear_bindings(m_stmt.get()) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + ": " +
                             sqlite3_errmsg(m_db_ref));
  }
};

int Sqlite3Statemnet::column_count() const {
  return sqlite3_column_count(m_stmt.get());
};

int Sqlite3Statemnet::column_int(int index) const {
  assert(index >= 0 && index < column_count());
  return sqlite3_column_int(m_stmt.get(), index);
};

int64_t Sqlite3Statemnet::column_int64(int index) const {
  assert(index >= 0 && index < column_count());
  return sqlite3_column_int64(m_stmt.get(), index);
};

double Sqlite3Statemnet::column_double(int index) const {
  assert(index >= 0 && index < column_count());
  return sqlite3_column_double(m_stmt.get(), index);
};

std::string Sqlite3Statemnet::column_text(int index) const {
  assert(index >= 0 && index < column_count());
  const auto *ptr =
      reinterpret_cast<const char *>(sqlite3_column_text(m_stmt.get(), index));
  return ptr ? std::string(ptr) : std::string();
};

bool Sqlite3Statemnet::column_is_null(int index) const {
  assert(index >= 0 && index < column_count());
  return sqlite3_column_type(m_stmt.get(), index) == SQLITE_NULL;
};

std::string Sqlite3Statemnet::column_name(int index) const {
  assert(index >= 0 && index < column_count());
  return sqlite3_column_name(m_stmt.get(), index);
};

void Sqlite3Statemnet::bind_one(int index, bool param) {
  if (sqlite3_bind_int(m_stmt.get(), index, param ? 1 : 0) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
};

void Sqlite3Statemnet::bind_one(int index, int param) {
  if (sqlite3_bind_int(m_stmt.get(), index, param) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
};

void Sqlite3Statemnet::bind_one(int index, int64_t param) {
  if (sqlite3_bind_int64(m_stmt.get(), index, param) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
};

void Sqlite3Statemnet::bind_one(int index, double param) {
  if (sqlite3_bind_double(m_stmt.get(), index, param) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
};

void Sqlite3Statemnet::bind_one(int index, std::string_view param) {
  if (sqlite3_bind_text(m_stmt.get(), index, param.data(), param.size(),
                        SQLITE_TRANSIENT) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
};

void Sqlite3Statemnet::bind_one(int index) {
  if (sqlite3_bind_null(m_stmt.get(), index) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
};
} // namespace ess::orm::core::sqlite3_impl
