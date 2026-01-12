#pragma once
#include <chrono>
#include <ess/orm/dialect.hpp>

namespace ess::orm::config {

struct DefaultConfig {

  using dialect = dialect::Sqlite;

  static constexpr bool enable_sql_logging = false;

  static constexpr std::size_t pool_size = 10;

  static constexpr std::chrono::milliseconds query_timeout{3000};
};

} // namespace ess::orm::config
