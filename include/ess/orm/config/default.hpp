#pragma once
#include <chrono>
#include <ess/orm/dialect.hpp>
#include <ess/orm/meta.hpp>

namespace ess::orm::config {

struct DefaultConfig {

  using dialect = dialect::Sqlite;

  // TODO: 以后可以考虑的差异化配置
  // struct DialectOptions {
  //   static constexpr bool use_ssl = true;
  //   static constexpr const char *schema_search_path = "public";
  // };
  //

  static constexpr std::string connection_url = "";

  static constexpr std::string password = "";

  static constexpr bool enable_sql_logging = false;

  static constexpr std::size_t pool_size = 10;

  static constexpr std::chrono::milliseconds query_timeout{3000};
};

} // namespace ess::orm::config
