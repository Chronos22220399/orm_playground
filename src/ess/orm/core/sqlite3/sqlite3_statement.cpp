#include "sqlite3_statement.h"
#include <ess/orm/common/error.hpp>

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

  // SQLite may return other codes that should be treated as errors
  const char *err_msg = sqlite3_errmsg(m_db_ref);
  std::string error_str = get_cur_loc_info() + ": ";

  // Check for specific error codes
  if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
    error_str += "database is busy/locked: ";
  } else if (rc == SQLITE_READONLY) {
    error_str += "database is read-only: ";
  } else if (rc == SQLITE_IOERR) {
    error_str += "I/O error: ";
  } else if (rc == SQLITE_CORRUPT) {
    error_str += "database corruption: ";
  } else if (rc == SQLITE_CONSTRAINT) {
    error_str += "constraint violation: ";
  } else if (rc == SQLITE_MISMATCH) {
    error_str += "data type mismatch: ";
  } else if (rc == SQLITE_MISUSE) {
    error_str += "library misuse: ";
  } else {
    error_str += "SQLite error (code ";
    error_str += std::to_string(rc);
    error_str += "): ";
  }

  error_str += (err_msg ? err_msg : "unknown error");
  throw std::runtime_error(error_str);
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

meta::ColumnType Sqlite3Statemnet::column_type(int index) const {
  int type = sqlite3_column_type(m_stmt.get(), index);
  switch (type) {
  case SQLITE_INTEGER:
    return meta::ColumnType::Int64;
  case SQLITE_FLOAT:
    return meta::ColumnType::Float;
  case SQLITE_TEXT:
    return meta::ColumnType::Text;
  case SQLITE_BLOB:
    return meta::ColumnType::Blob;
  case SQLITE_NULL:
    return meta::ColumnType::Null;
  default:
    return meta::ColumnType::Unknown;
  }
}

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

void Sqlite3Statemnet::bind_one(int index, const char *param) {
  if (sqlite3_bind_text(m_stmt.get(), index, param, -1, SQLITE_TRANSIENT) !=
      SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
}

void Sqlite3Statemnet::bind_one(int index) {
  if (sqlite3_bind_null(m_stmt.get(), index) != SQLITE_OK) {
    throw std::runtime_error(get_cur_loc_info() + sqlite3_errmsg(m_db_ref));
  }
};
} // namespace ess::orm::core::sqlite3_impl
