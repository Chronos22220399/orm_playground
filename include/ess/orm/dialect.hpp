#pragma once
#include <ess/orm/meta.hpp>

namespace ess::orm::dialect {

struct DialectTag {};

struct Sqlite3 : DialectTag {};

struct Postgres : DialectTag {};

template <typename Database>
concept dialect_type = std::derived_from<Database, DialectTag>;

template <dialect_type Database, meta::FixedString SQL> static bool validate() {
  return true;
}
} // namespace ess::orm::dialect
