#pragma once
#include <ess/orm/connection_pool.h>
#include <ess/orm/context.hpp>
#include <ess/orm/parser.hpp>
#include <ess/orm/result.h>
#include <ess/orm/result_set_mapper.hpp>
#include <thread>

namespace ess::orm {

struct Read {};

struct Write {};

enum class TxMode { READ, WRITE };

template <typename T>
concept transaction_mode = std::is_same_v<T, Read> || std::is_same_v<T, Write>;

// 事务上下文
class TransactionContext {
  std::shared_ptr<Connection> m_conn = nullptr;
  std::thread::id m_thread_id;
  std::type_index m_db_type;
  int m_nesting_level = 0;
  TxMode m_mode;

public:
  TransactionContext(std::shared_ptr<Connection> conn, std::type_index db_type,
                     TxMode mode)
      : m_conn(std::move(conn)), m_thread_id(std::this_thread::get_id()),
        m_db_type(db_type), m_mode(mode) {}

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

  TxMode mode() { return m_mode; }
};

// 全局统一事务上下文管理器
class ESS_ORM_API TransactionContextManager {
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

  TransactionContext *get_default() const { return get<config::default_db>(); }

  bool in_any_transaction() const { return !m_contexts.empty(); }
};

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
template <transaction_mode Mode = Write,
          concepts::database_type DB = config::default_db>
class Transaction {
  std::shared_ptr<Connection> m_conn;
  int &m_nesting_level;
  int m_my_level;
  bool m_committed = false;
  std::thread::id m_owner_thread;

  class TransactionGuard {
    Transaction<Mode, DB> &m_tx;

  public:
    explicit TransactionGuard(Transaction<Mode, DB> &tx) : m_tx(tx) {
      m_tx.begin();
    }

    TransactionGuard(TransactionGuard const &) = delete;

    TransactionGuard &operator=(TransactionGuard const &) = delete;

    ~TransactionGuard() { m_tx.commit(); }
  };

  void verify_thread() const {
    if (std::this_thread::get_id() != m_owner_thread) {
      throw std::runtime_error(
          "Transaction object is being accessed from a different thread. "
          "SQLite connections are not thread-safe.");
    }
  }

private:
  template <concepts::table_type Table, parser::SqlType sql_type>
  constexpr auto valide_query() const {
    static_assert(std::is_same_v<typename Table::Database, DB>,
                  "Database type missmatched, in query<Table,...>, where "
                  "Table::Database must equal to transaction<DB>'s DB");

    static_assert(concepts::table_type<Table>,
                  "请使用持有 Schema 的 Table 类型");

    if constexpr (std::is_same_v<Mode, Read>) {
      static_assert(sql_type == parser::SqlType::SELECT,
                    "Read transaction can not use 'DELETE/INSERT/UPDATE'. "
                    "Use Write transaction<DB, Write> instead.");
    }

    static_assert(sql_type != parser::SqlType::UNKNOWN,
                  "Unsupported or Invalid SQL statement");
  }

  struct AsEntity {};
  struct AsRow {};

  template <typename ResultTag, concepts::table_type Table,
            meta::FixedString Sql, typename... Args>
  auto query_impl(Args &&...args) {
    verify_thread();
    using namespace parser;

    // constexpr auto sql = meta::fs_to_upper(Sql);
    constexpr auto sql = Sql;
    constexpr auto sql_type = parser::begin_with<sql>(); // 编译时常量

    valide_query<Table, sql_type>();

    Statement &stmt = m_conn->prepare_cached(sql);
    auto scope = stmt.scope_guard();
    stmt.bind_params(std::forward<Args>(args)...);

    ResultSetMapper<Table> mapper;
    mapper.init_mapper(stmt.get());

    if constexpr (sql_type == SqlType::SELECT) {
      return execute_select<ResultTag, Table>(stmt);
    } else if constexpr (sql_type == SqlType::INSERT) {
      stmt.next();
      return InsertResult{.last_insert_id =
                              sqlite3_last_insert_rowid(m_conn->handle()),
                          .affected_rows = sqlite3_changes(m_conn->handle())};
    } else if constexpr (sql_type == SqlType::UPDATE ||
                         sql_type == SqlType::DELETE) {
      stmt.next();
      return ModifyResult{.affected_rows = sqlite3_changes(m_conn->handle())};
    }
  }

  template <typename ResultTag, concepts::table_type Table>
  auto execute_select(Statement &stmt) {
    ResultSetMapper<Table> mapper;
    mapper.init_mapper(stmt.get());

    if constexpr (std::is_same_v<ResultTag, AsEntity>) {
      std::vector<Table> res{};
      res.reserve(16);
      while (stmt.next()) {
        res.emplace_back();
        mapper.map_row(stmt.get(), res.back());
      }
      return res;
    } else {
      std::vector<Row> res{};
      res.reserve(16);
      while (stmt.next()) {
        res.push_back(mapper.map_row(stmt.get()));
      }
      return res;
    }
  }

public:
  Transaction(std::shared_ptr<Connection> conn, int &nesting_level)
      : m_conn(std::move(conn)), m_nesting_level(nesting_level),
        m_my_level(nesting_level), m_owner_thread(std::this_thread::get_id()) {}

  ~Transaction() {}

  void begin() {
    verify_thread();
    if (m_my_level == 0) {
      if constexpr (std::is_same_v<Mode, Write>) {
        m_conn->execute_raw("BEGIN IMMEDIATE");
      } else {
        m_conn->execute_raw("BEGIN DEFERRED");
      }
    } else {
      m_conn->execute_raw(fmt::format("SAVEPOINT sp_{}", m_my_level));
    }
    ++m_nesting_level;
  }

  void commit() {
    verify_thread();
    --m_nesting_level;
    if (m_my_level == 0) {
      m_conn->execute_raw("COMMIT");
    } else {
      m_conn->execute_raw(fmt::format("RELEASE SAVEPOINT sp_{}", m_my_level));
    }
    m_committed = true;
  }

  void rollback() {
    verify_thread();
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

  std::shared_ptr<Connection> shared_connection() { return m_conn; }

  [[nodiscard]] TransactionGuard scope_guard() {
    return TransactionGuard{*this};
  }

  template <meta::FixedString Sql> auto exec() {
    verify_thread();
    Statement &stmt = m_conn->execute_raw(Sql);
  }

  template <concepts::table_type Table, meta::FixedString Sql, typename... Args>
  auto query(Args &&...args) {
    return query_impl<AsEntity, Table, Sql>(std::forward<Args>(args)...);
  }

  template <concepts::table_type Table, meta::FixedString Sql, typename... Args>
  auto query_rows(Args &&...args) {
    return query_impl<AsRow, Table, Sql>(std::forward<Args>(args)...);
  }

  // TODO: 待改进
  template <concepts::table_type Table, meta::FixedString Sql, typename R,
            typename... Args>
  auto query_scaler(return_type<R>, Args &&...args) {

    verify_thread();
    using namespace parser;

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
};

template <typename Func, typename Mode, typename DB>
concept explicit_tx_func = std::invocable<Func, Transaction<Mode, DB> &>;

// 显式模式
template <transaction_mode Mode = Write,
          concepts::database_type DB = config::default_db, typename Func>
  requires explicit_tx_func<Func, Mode, DB>
auto transaction(Func &&func) {
  // 检查是否有当前数据库连接的上下文
  TransactionContext *current_ctx =
      TransactionContextManager::instance.get<DB>();
  // 判断是否为跟事务
  const bool is_root = (current_ctx == nullptr);

  constexpr bool is_read = std::is_same_v<Mode, Read>;
  if (!is_root) {
    if constexpr (is_read) {
      if (current_ctx->mode() == TxMode::WRITE) {
        throw std::runtime_error(
            "Can't nest read transaction inside write transaction, Use write "
            "transaction or move read outside.\n");
      }
    }
  }

  std::unique_ptr<TransactionContext> ctx_owner;
  std::shared_ptr<Connection> conn_ptr;

  if (is_root) {
    conn_ptr = Context::instance().conn_pool<DB>().acquire();
    ctx_owner = std::make_unique<TransactionContext>(
        conn_ptr, std::type_index(typeid(DB)),
        (is_read ? TxMode::READ : TxMode::WRITE));
    current_ctx = ctx_owner.get();
  } else {
    conn_ptr = current_ctx->shared_connection();
  }

  // 检查是否跨线程
  current_ctx->verify_thread();

  // 上下文管理恢复用
  std::optional<ContextGuard<DB>> ctx_guard;
  if (is_root) {
    ctx_guard.emplace(current_ctx);
  }

  Transaction<Mode, DB> tx =
      Transaction<Mode, DB>(conn_ptr, current_ctx->nesting_level());

  tx.begin();
  try {
    using ret_type = std::invoke_result_t<Func, Transaction<Mode, DB> &>;
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
    // TODO: 后续可通过在错误中添加对数据库类型的比较实现彻底的数据库事务隔离
    // 当前的实现下，嵌套的数据库A的事务rollback后，抛出的错误会直接影响到外层的事务，会让外层随之rollback，后续可按照todo的更改
    throw;
  }
}

} // namespace ess::orm
