#pragma once
#include <ess/orm/config/config.hpp>
#include <ess/orm/connection_pool.h>
#include <ess/orm/context.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/dsl.hpp>
#include <ess/orm/parser.hpp>
#include <ess/orm/result.h>
#include <ess/orm/result_set_mapper.hpp>
#include <ess/orm/row.h>
#include <ess/orm/sql/validator.hpp>
#include <fmt/format.h>

namespace ess::orm {

template <concepts::table_type Table, meta::FixedString Sql, typename... Args>
auto query(Args &&...args) {
  static_assert(concepts::table_type<Table>, "请使用持有 Schema 的 Table 类型");
  // 静态校验
  using namespace parser;
  using DB = Table::Database;

  // TODO: 完善
  // constexpr auto sql = meta::fs_to_upper(Sql);
  constexpr auto sql = Sql;
  constexpr auto sql_type = parser::begin_with<sql>(); // 编译时常量

  std::shared_ptr<Connection> conn =
      Context::instance().conn_pool<DB>().acquire();

  Statement &stmt = conn->prepare_cached(sql);
  auto scope = stmt.scope_guard();
  stmt.bind_params(std::forward<Args>(args)...);

  ResultSetMapper<Table> mapper;
  mapper.init_mapper(stmt.get());

  if constexpr (sql_type == SqlType::SELECT) {
    std::vector<Table> res{};
    res.reserve(16);

    while (stmt.next()) {
      res.emplace_back();
      mapper.map_row(stmt.get(), res.back());
    }

    return res;

  } else if constexpr (sql_type == SqlType::INSERT) {
    stmt.next();
    return InsertResult{.last_insert_id =
                            sqlite3_last_insert_rowid(conn->handle()),
                        .affected_rows = sqlite3_changes(conn->handle())};
  } else if constexpr (sql_type == SqlType::UPDATE ||
                       sql_type == SqlType::DELETE) {
    stmt.next();
    return ModifyResult{.affected_rows = sqlite3_changes(conn->handle())};

  } else {
    // 编译时错误，而不是运行时抛异常
    static_assert(sql_type != SqlType::UNKNOWN,
                  "\nUnsupported or Invalid SQL statement\n");
  }
}

template <concepts::table_type Table, meta::FixedString Sql, typename... Args>
std::vector<Row> query_row(Args &&...args) {
  static_assert(concepts::table_type<Table>, "请使用持有 Schema 的 Table 类型");
  // 静态校验
  using namespace parser;
  using DB = Table::Database;

  // TODO: 完善
  // constexpr auto sql = meta::fs_to_upper(Sql);
  constexpr auto sql = Sql;
  constexpr auto sql_type = parser::begin_with<sql>(); // 编译时常量

  std::shared_ptr<Connection> conn =
      Context::instance().conn_pool<DB>().acquire();

  Statement &stmt = conn->prepare_cached(sql);
  auto scope = stmt.scope_guard();
  stmt.bind_params(std::forward<Args>(args)...);

  ResultSetMapper<Table> mapper;
  mapper.init_mapper(stmt.get());

  if constexpr (sql_type == SqlType::SELECT) {
    std::vector<Row> res{};
    res.reserve(16);

    while (stmt.next()) {
      res.emplace_back(mapper.map_row(stmt.get()));
    }

    return res;
  } else {
    // 编译时错误，而不是运行时抛异常
    static_assert(sql_type != SqlType::UNKNOWN,
                  "\nUnsupported or Invalid SQL statement\n");
  }
}

// TODO: 待改进
template <concepts::table_type Table, meta::FixedString Sql, typename R,
          typename... Args>
auto query_scaler(return_type<R>, Args &&...args) {
  static_assert(concepts::table_type<Table>, "请使用持有 Schema 的 Table 类型");
  // 静态校验
  using namespace parser;
  using DB = Table::Database;

  // constexpr auto sql = meta::fs_to_upper(Sql);
  constexpr auto sql = Sql;
  constexpr auto sql_type = parser::begin_with<sql>(); // 编译时常量

  static_assert(sql_type == parser::SqlType::SELECT,
                "query_scaler only support select.");

  std::shared_ptr<Connection> conn =
      Context::instance().conn_pool<DB>().acquire();

  Statement &stmt = conn->prepare_cached(sql);
  auto scope = stmt.scope_guard();
  stmt.bind_params(std::forward<Args>(args)...);

  if (stmt.next()) {
    return get_column<R>(stmt.get(), 0);
  }
  // 没有结果则返回默认值或者抛出异常
  return R{};
}

} // namespace ess::orm
