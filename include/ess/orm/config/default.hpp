#pragma once
#include <chrono>
#include <ess/orm/dialect.hpp>
#include <ess/orm/meta.hpp>

namespace ess::orm::config {
namespace inner_default {

struct DefaultDB {
  // 必填
  static constexpr std::string connection_url = "./data/test.db";
  // sqlite3 选填，mysql、postgres 必填
  static constexpr std::string password = "";
  // 选填
  static constexpr std::size_t pool_size = 10;
  static constexpr std::chrono::milliseconds query_timeout{3000};
  static constexpr std::chrono::milliseconds busy_timeout{30000};
  static constexpr bool enable_sql_logging = false;
  static constexpr bool enable_wal_mode = true;
};

struct DefaultGlobalConfig {

  using dialect = dialect::Sqlite3;

  // TODO: 添加多个数据库
  using databases = std::tuple<DefaultDB>;

  using default_db = DefaultDB;
};
} // namespace inner_default

} // namespace ess::orm::config
