#pragma once
#include <ess/orm/meta.hpp>

namespace ess::orm::dialect {

namespace impl {
struct DialectTag {};
} // namespace impl

struct Sqlite3 : impl::DialectTag {};

struct Mysql : impl::DialectTag {};

struct Postgres : impl::DialectTag {};

template <typename Database>
concept dialect_type = std::derived_from<Database, impl::DialectTag>;

template <dialect_type Database, meta::FixedString SQL> static bool validate() {
  return true;
}
} // namespace ess::orm::dialect
