#pragma once
#include <ess/orm/meta.hpp>

namespace ess::orm::dialect {

struct DialectTag {};

struct Sqlite : DialectTag {};

struct Postgres : DialectTag {};

constexpr Sqlite get_orm_dialect(...) { return {}; }

template <typename Dialect>
concept dialect_type = std::derived_from<Dialect, DialectTag>;

template <dialect_type Dialect, meta::FixedString SQL> static bool validate() {
  return true;
}
} // namespace ess::orm::dialect
