#pragma once
#include <ess/orm/defines.h>
// #include <memory>
// #include <sqlite3.h>

struct sqlite3;
struct sqlite3_stmt;

namespace ess::orm {

struct ESS_ORM_API SqliteDestroier {

  void operator()(sqlite3 *db) const;

  void operator()(sqlite3_stmt *stmt) const;

  void operator()(char *expanded_sql) const;
};

} // namespace ess::orm
