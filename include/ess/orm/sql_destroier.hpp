#pragma once
#include <memory>
#include <sqlite3.h>

namespace ess::orm {

struct SqliteDestroier {

  void operator()(sqlite3 *db) const {
    if (db)
      sqlite3_close(db);
  }

  void operator()(sqlite3_stmt *stmt) const {
    if (stmt)
      sqlite3_finalize(stmt);
  }

  void operator()(char *expanded_sql) const {
    if (expanded_sql)
      sqlite3_free((void *)expanded_sql);
  }
};

using SqlitePtr = std::unique_ptr<sqlite3, SqliteDestroier>;

using StatPtr = std::unique_ptr<sqlite3_stmt, SqliteDestroier>;

using ExpanedSqlPtr = std::unique_ptr<char, SqliteDestroier>;

} // namespace ess::orm
