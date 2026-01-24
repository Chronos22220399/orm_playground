#pragma once
#include <ess/orm/connection_pool.h>
#include <ess/orm/context.hpp>
#include <ess/orm/parser.hpp>
#include <ess/orm/result_set_mapper.hpp>

namespace ess::orm {

// 事务
template <concepts::database_type DB = config::DefaultDB> class Transaction {
  ConnectionPool::ConnectionGuard m_conn_guard;
  int &m_nesting_level;
  int m_my_level;
  bool m_committed = false;

public:
  Transaction(ConnectionPool::ConnectionGuard guard, int &nesting_level)
      : m_conn_guard(std::move(guard)), m_nesting_level(nesting_level),
        m_my_level(nesting_level) {}

  ~Transaction() {}

  void begin() {
    if (m_my_level == 0) {
      m_conn_guard->execute_raw("BEGIN IMMEDIATE");
    } else {
      m_conn_guard->execute_raw(fmt::format("SAVEPOINT sp_{}", m_my_level));
    }
    ++m_nesting_level;
  }

  void commit() {
    --m_nesting_level;
    if (m_my_level == 0) {
      m_conn_guard->execute_raw("COMMIT");
    } else {
      m_conn_guard->execute_raw(
          fmt::format("RELEASE SAVEPOINT sp_{}", m_my_level));
    }
    m_committed = true;
  }

  void rollback() {
    if (m_committed)
      return;
    --m_nesting_level;
    if (m_my_level == 0) {
      m_conn_guard->execute_raw("ROLLBACK");
    } else {
      m_conn_guard->execute_raw(
          fmt::format("ROLLBACK TO SAVEPOINT sp_", m_my_level));
    }
  }

  Connection &connection() { return *m_conn_guard; }

  std::shared_ptr<Connection> shared_connection() {
    return std::move(m_conn_guard.shared());
  }

  template <concepts::table_type Table, meta::FixedString Sql, typename... Args>
  auto query(Args &&...args) {
    using namespace parser;

    Statement &stmt = m_conn_guard->prepare_cached(Sql);
    auto scope = stmt.scope_guard();
    stmt.bind_params(std::forward<Args>(args)...);

    ResultSetMapper<Table> mapper;
    mapper.init_mapper(stmt.get());

    switch (parser::begin_with<Sql>()) {
    case SqlType::SELECT: {
      // TODO: 后续用自定义结果集容器代替 vector
      std::vector<Table> res{};
      res.reserve(16);

      while (stmt.next()) {
        res.emplace_back();
        mapper.map_row(stmt.get(), res.back());
      }
      return res;
    }
    case SqlType::INSERT: {
    }
    case SqlType::UPDATE: {
    }
    case SqlType::DELETE: {
      stmt.next();
      sqlite3_changes(m_conn_guard->handle());
    }
    case SqlType::UNKNOWN: {
      throw std::runtime_error("Unsupported or Invalid SQL statement");
    }
    }
  }
};

// TODO: 2026/1/26 todo
template <concepts::table_type Table, meta::FixedString Sql, typename... Args>
auto query() {}

auto transaction() {}

} // namespace ess::orm
