#pragma once
#include <ess/orm/config/config.hpp>
#include <ess/orm/connection_pool.h>
#include <ess/orm/dialect.hpp>
#include <ess/orm/dsl.hpp>
#include <ess/orm/row.h>
#include <fmt/format.h>

namespace ess::orm {

template <typename Func> auto transaction(Func &&func) {}

template <typename Table, meta::FixedString SQL> auto query(auto &&...args) {
  static_assert(dsl::table_type<Table>, "请使用持有 Schema 的 Table 类型");
  // 静态校验

  if (std::is_same_v<config::dialect, dialect::Postgres>) {
    fmt::println("Postgres");
  } else if (std::is_same_v<config::dialect, dialect::Sqlite3>) {
    fmt::println("Sqlite3");
  }
}

} // namespace ess::orm
