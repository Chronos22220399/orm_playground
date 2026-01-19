#pragma once
#include <ess/orm/config/traits.hpp>
#include <ess/orm/dialect.hpp>
#include <fmt/color.h>

namespace ess::orm::config {

// 用户配置检测
#if __has_include(<ess_orm_user_config.hpp>)
#include <ess_orm_user_config.hpp>
inline constexpr bool has_user_config = true;
#else
struct UserConfig {};
inline constexpr bool has_user_config = false;
#endif

// 最终配置

using config = ConfigTrait<UserConfig, DefaultConfig>;

using dialect = config::dialect;

static constexpr std::string connection_url = config::connection_url;

static constexpr bool enable_sql_logging = config::enable_sql_logging;

static constexpr std::size_t pool_size = config::pool_size;

static constexpr std::chrono::milliseconds query_timeout =
    config::query_timeout;

static constexpr std::string get_dialect_string() {
  if constexpr (std::is_same_v<dialect, orm::dialect::Sqlite3>) {
    return "Sqlite3";
  } else if constexpr (std::is_same_v<dialect, orm::dialect::Postgres>) {
    return "Postgres";
  } else {
    throw;
  }
}

static void print_config() {
  fmt::print(fmt::fg(fmt::color::cyan),
             "\n[Ess ORM Configuration]\n"
             "  {:<18} {}\n"
             "  {:<18} {}\n"
             "  {:<18} {}\n"
             "  {:<18} {}\n"
             "  {:<18} {}ms\n",
             "User config:", has_user_config ? "detected" : "not detected",
             "Dialect:", get_dialect_string(),
             "SQL Logging:", enable_sql_logging ? "true" : "false",
             "Pool size:", pool_size, "Query timeout:", query_timeout.count());
}

} // namespace ess::orm::config
