#pragma once
#include <ess/orm/connection_pool.h>
#include <ess/orm/context.hpp>
#include <ess/orm/parser.hpp>
#include <ess/orm/result_set_mapper.hpp>
#include <thread>

namespace ess::orm {

// 事务上下文
class TransactionContext {
  std::shared_ptr<Connection> m_conn = nullptr;
  std::thread::id m_thread_id;
  std::type_index m_db_type;
  int m_nesting_level = 0;

public:
  TransactionContext(std::shared_ptr<Connection> conn, std::type_index db_type)
      : m_conn(std::move(conn)), m_thread_id(std::this_thread::get_id()),
        m_db_type(db_type) {}

  std::shared_ptr<Connection> shared_connection() const { return m_conn; }

  void verify_thread() const {
    if (std::this_thread::get_id() != m_thread_id) {
      throw std::runtime_error("Transaction access from different thread\n"
                               "Hint: Use explicit mode `transaction([](auto& "
                               "tx) { tx.query<...>(); })`\n"
                               "for cross-thread scenarios\n");
    }
  }

  int &nesting_level() { return m_nesting_level; }
};

// 全局统一事务上下文管理器
class TransactionContextManager {
  std::unordered_map<std::type_index, TransactionContext *> m_contexts;

public:
  static thread_local TransactionContextManager instance;

  template <concepts::database_type DB> void set(TransactionContext *ctx) {
    m_contexts[std::type_index(typeid(DB))] = ctx;
  }

  template <concepts::database_type DB> void clear() {
    m_contexts.erase(std::type_index(typeid(DB)));
  }

  template <concepts::database_type DB> TransactionContext *get() const {
    auto it = m_contexts.find(std::type_index(typeid(DB)));
    return it != m_contexts.end() ? it->second : nullptr;
  }

  TransactionContext *get_default() const { return get<config::DefaultDB>(); }

  bool in_any_transaction() const { return !m_contexts.empty(); }
};

inline thread_local TransactionContextManager
    TransactionContextManager::instance;

// 事务 guard
template <concepts::database_type DB> class ContextGuard {
  TransactionContext *m_prev;

public:
  ContextGuard(TransactionContext *current)
      : m_prev(TransactionContextManager::instance.get<DB>()) {
    TransactionContextManager::instance.set<DB>(current);
  }

  ContextGuard(ContextGuard const &&) = delete;
  ContextGuard &operator=(ContextGuard const &&) = delete;

  ~ContextGuard() {
    if (m_prev) {
      TransactionContextManager::instance.set<DB>(m_prev);
    } else {
      TransactionContextManager::instance.clear<DB>();
    }
  }
};

// 事务
template <concepts::database_type DB = config::DefaultDB> class Transaction {
  std::shared_ptr<Connection> m_conn;
  int &m_nesting_level;
  int m_my_level;
  bool m_committed = false;

  class TransactionGuard {
    Transaction<DB> &m_tx;

  public:
    explicit TransactionGuard(Transaction<DB> &tx) : m_tx(tx) { m_tx.begin(); }

    TransactionGuard(TransactionGuard const &) = delete;

    TransactionGuard &operator=(TransactionGuard const &) = delete;

    ~TransactionGuard() { m_tx.commit(); }
  };

public:
  Transaction(std::shared_ptr<Connection> guard, int &nesting_level)
      : m_conn(std::move(guard)), m_nesting_level(nesting_level),
        m_my_level(nesting_level) {}

  ~Transaction() {}

  void begin() {
    if (m_my_level == 0) {
      m_conn->execute_raw("BEGIN IMMEDIATE");
    } else {
      m_conn->execute_raw(fmt::format("SAVEPOINT sp_{}", m_my_level));
    }
    ++m_nesting_level;
  }

  void commit() {
    --m_nesting_level;
    if (m_my_level == 0) {
      m_conn->execute_raw("COMMIT");
    } else {
      m_conn->execute_raw(fmt::format("RELEASE SAVEPOINT sp_{}", m_my_level));
    }
    m_committed = true;
  }

  void rollback() {
    if (m_committed)
      return;
    --m_nesting_level;
    if (m_my_level == 0) {
      m_conn->execute_raw("ROLLBACK");
    } else {
      m_conn->execute_raw(
          fmt::format("ROLLBACK TO SAVEPOINT sp_{}", m_my_level));
    }
  }

  Connection &connection() { return *m_conn; }

  std::shared_ptr<Connection> shared_connection() { return std::move(m_conn); }

  [[nodiscard]] TransactionGuard scope_guard() {
    return TransactionGuard{*this};
  }

  template <concepts::table_type Table, meta::FixedString Sql, typename... Args>
  auto query(Args &&...args) {
    using namespace parser;

    constexpr auto sql = meta::fs_to_upper(Sql);

    Statement &stmt = m_conn->prepare_cached(sql);
    auto scope = stmt.scope_guard();
    stmt.bind_params(std::forward<Args>(args)...);

    ResultSetMapper<Table> mapper;
    mapper.init_mapper(stmt.get());

    constexpr auto sql_type = parser::begin_with<sql>(); // 编译时常量

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
      return sqlite3_last_insert_rowid(m_conn->handle());

    } else if constexpr (sql_type == SqlType::UPDATE ||
                         sql_type == SqlType::DELETE) {
      stmt.next();
      return sqlite3_changes(m_conn->handle());

    } else {
      // 编译时错误，而不是运行时抛异常
      static_assert(sql_type != SqlType::UNKNOWN,
                    "Unsupported or Invalid SQL statement");
    }
  }
};

template <typename Func, typename DB>
concept explicit_tx_func = std::invocable<Func, Transaction<DB> &>;

template <typename Func>
concept implicit_tx_func =
    std::invocable<Func> &&
    (!std::invocable<Func, Transaction<config::DefaultDB> &>);

// 显式模式
template <concepts::database_type DB = config::DefaultDB, typename Func>
  requires explicit_tx_func<Func, DB>
auto transaction(Func &&func) {
  // 检查是否有当前数据库连接的上下文
  TransactionContext *current_ctx =
      TransactionContextManager::instance.get<DB>();
  // 判断是否为跟事务
  const bool is_root = (current_ctx == nullptr);

  std::unique_ptr<TransactionContext> ctx_owner;
  std::optional<ConnectionPool::ConnectionGuard> conn_guard;
  std::shared_ptr<Connection> conn_ptr = nullptr;

  if (is_root) {
    conn_guard = Context::instance().conn_pool<DB>().acquire();
    if (!conn_guard.has_value()) {
      throw std::runtime_error(
          fmt::format("Can't acquire a connection from pool, the "
                      "relevant database path is: {}\n",
                      DB::connection_url));
    }
    conn_ptr = conn_guard.value().shared();
    ctx_owner = std::make_unique<TransactionContext>(
        conn_ptr, std::type_index(typeid(DB)));
    current_ctx = ctx_owner.get();
  } else {
    conn_ptr = current_ctx->shared_connection();
  }

  // 上下文管理恢复用
  std::optional<ContextGuard<DB>> ctx_guard;
  if (is_root) {
    ctx_guard.emplace(current_ctx);
  }

  Transaction<DB> tx = Transaction<DB>(conn_ptr, current_ctx->nesting_level());

  tx.begin();
  try {
    using ret_type = std::invoke_result_t<Func, Transaction<DB> &>;
    if constexpr (std::is_void_v<ret_type>) {
      std::invoke(std::forward<Func>(func), tx);
      tx.commit();
      return;
    } else {
      auto result = std::invoke(std::forward<Func>(func), tx);
      tx.commit();
      return result;
    }
  } catch (...) {
    tx.rollback();
    throw;
  }
}

// 隐式模式
// 1. 获取当前上下文
// 2. 上下文为空则创建并注册上下文，否则直接使用上下文的内容
// 3. 将上下文的内容交给guard保存（RAII）
// 4. 通过上下文

// TODO: 2026/1/26 todo
// template <concepts::table_type Table, meta::FixedString Sql, typename...
// Args> auto query() {}

} // namespace ess::orm
