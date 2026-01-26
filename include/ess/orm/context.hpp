#pragma once
#include <ess/orm/common_concept.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/config/default.hpp>
#include <ess/orm/connection.h>
#include <ess/orm/connection_pool.h>
#include <ess/orm/statement.h>
#include <typeindex>
#include <unordered_map>

namespace ess::orm {

// 全局 Context
class Context {
  std::unordered_map<std::type_index, std::unique_ptr<ConnectionPool>> m_pools =
      {};
  std::once_flag m_init_flag{};

  Context() { init(); };

  template <concepts::database_type DB> void register_db() {
    using trait = config::DatabaseTrait<DB, config::default_db_config>;
    auto pool = std::make_unique<ConnectionPool>(trait::connection_url,
                                                 trait::pool_size);
    m_pools[std::type_index(typeid(DB))] = std::move(pool);
  }

public:
  static Context &instance() {
    static Context ctx;
    return ctx;
  }

  // TODO: 后续自动注册用
  void init() {
    std::call_once(m_init_flag, [this]() {
      // init database
      using databases = config::databases;
      [this]<std::size_t... I>(std::index_sequence<I...>) {
        (register_db<std::tuple_element_t<I, databases>>(), ...);
      }(std::make_index_sequence<config::database_count>{});
    });
  }

  template <concepts::database_type DB = config::default_db>
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
};

} // namespace ess::orm
