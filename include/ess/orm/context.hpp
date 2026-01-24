#pragma once
#include <ess/orm/common_concept.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/config/default.hpp>
#include <ess/orm/connection.h>
#include <ess/orm/connection_pool.h>
#include <ess/orm/statement.h>
#include <thread>
#include <typeindex>
#include <unordered_map>

struct MainDB {};

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

  void verify_thread() const {
    if (std::this_thread::get_id() != m_thread_id) {
      throw std::runtime_error("Transaction access from different thread\n"
                               "Hint: Use explicit mode `transaction([](auto& "
                               "tx) { tx.query<...>(); })`\n"
                               "for cross-thread scenarios\n");
    }
  }
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
template <concepts::database_type DB> class TransactionGuard {
  TransactionContext *m_prev;

public:
  TransactionGuard(TransactionContext *current)
      : m_prev(TransactionContextManager::instance.get<DB>()) {
    TransactionContextManager::instance.set<DB>(current);
  }

  ~TransactionGuard() {
    if (m_prev) {
      TransactionContextManager::instance.set<DB>(m_prev);
    } else {
      TransactionContextManager::instance.clear<DB>();
    }
  }
};

// 全局 Context
class Context {
  std::unordered_map<std::type_index, std::unique_ptr<ConnectionPool>> m_pools =
      {};
  std::once_flag m_init_flag{};

  Context() = default;

public:
  static Context &instance() {
    static Context ctx;
    return ctx;
  }

  template <concepts::database_type DB = config::DefaultDB>
  void register_db(std::string_view connection_url, std::size_t pool_size) {
    auto pool = std::make_unique<ConnectionPool>(connection_url, pool_size);
    m_pools[std::type_index(typeid(DB))] = std::move(pool);
  }

  template <concepts::database_type DB = config::DefaultDB>
  ConnectionPool &conn_pool() {
    auto it = m_pools.find(std::type_index(typeid(DB)));
    if (it == m_pools.end()) {
      // TODO:
      // 后续会改成自动注册，因此错误信息需要更改成去配置文件配置相关信息等
      throw std::runtime_error("Database not registered. \n"
                               "Call Context::instance().register_db<DB>() or\n"
                               "Context::instance().init() first.");
    }
    return *(it->second);
  }

  // TODO: 后续自动注册用
  void init() {
    std::call_once(m_init_flag, [this]() {
      register_db(config::connection_url, config::pool_size);
    });
  }
};

} // namespace ess::orm
