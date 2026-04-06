#pragma once
#include <ess/orm/common/concept.hpp>
#include <ess/orm/common/defines.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/config/default.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/connection_pool.hpp>
#include <ess/orm/core/statement.hpp>
#include <mutex>
#include <typeindex>
#include <unordered_map>

namespace ess::orm {

// 全局 Context
class ESS_ORM_API Context {
  std::unordered_map<std::type_index,
                     std::shared_ptr<core::ConnectionPool<config::dialect>>>
      m_pools = {};
  std::once_flag m_init_flag{};

  Context() {};

public:
  static Context &instance();

  void init() {
    using dbs = config::databases;
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      (register_db<std::tuple_element_t<I, dbs>>(), ...);
    }(std::make_index_sequence<std::tuple_size_v<dbs>>{});
  }

  // 用户可以手动注册自己的数据库配置
  template <concepts::database_type DB> void register_db() {
    using trait = config::DatabaseTrait<DB, config::default_db_config>;
    auto pool = core::ConnectionPool<config::dialect>::create(
        trait::connection_url, trait::pool_size);
    m_pools[std::type_index(typeid(DB))] = std::move(pool);
  }

  template <concepts::database_type DB = config::default_db>
  core::ConnectionPool<config::dialect> &conn_pool() {
    auto it = m_pools.find(std::type_index(typeid(DB)));
    if (it == m_pools.end()) {
      throw std::runtime_error(
          "Database not registered. \n"
          "Call Context::instance().register_db<DB>() first.");
    }
    return *(it->second);
  }
};

} // namespace ess::orm
