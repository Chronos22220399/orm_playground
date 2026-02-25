#include <ess/orm/sql_destroier.hpp>
#include <sqlite3.h>

namespace ess::orm {

void SqliteDestroier::operator()(sqlite3 *db) const {
  if (db)
    sqlite3_close(db);
}

void SqliteDestroier::operator()(sqlite3_stmt *stmt) const {
  if (stmt)
    sqlite3_finalize(stmt);
}

void SqliteDestroier::operator()(char *expanded_sql) const {
  if (expanded_sql)
    sqlite3_free((void *)expanded_sql);
}
} // namespace ess::orm
