#pragma once
#include <chrono>
#include <ess/orm/common/meta.hpp>
#include <ess/orm/core/dialect.hpp>

namespace ess::orm::config {
namespace inner_default {

struct DefaultDB {
  // 必填
  static constexpr std::string_view connection_url = "";
  // sqlite3 选填，mysql、postgres 必填
  static constexpr std::string_view password = "";
  // 选填
  static constexpr std::size_t pool_size = 10;
  static constexpr std::chrono::milliseconds query_timeout{3000};
  static constexpr std::chrono::milliseconds busy_timeout{30000};
  static constexpr bool enable_sql_logging = false;
  static constexpr bool enable_wal_mode = true;
};

struct DefaultGlobalConfig {

  using dialect = dialect::Sqlite3;

  using databases = std::tuple<DefaultDB>;

  using default_db = DefaultDB;

  // default result set container size
  static constexpr std::size_t default_container_size = 100;
};
} // namespace inner_default

} // namespace ess::orm::config
