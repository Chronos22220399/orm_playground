#pragma once
#include <ess/orm/config/traits.hpp>
#include <ess/orm/core/dialect.hpp>
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

using default_db_config = inner_default::DefaultDB;

using Config =
    GlobalConfigTrait<UserConfig, inner_default::DefaultGlobalConfig>;
using dialect = Config::dialect;
using databases = Config::databases;
using default_db = Config::default_db;
static constexpr std::size_t database_count = Config::database_count;
static constexpr std::size_t default_container_size =
    Config::default_container_size;

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
             "  {:<18} {}\n",
             "User config:", has_user_config ? "detected" : "not detected",
             "Dialect:", get_dialect_string());
}

} // namespace ess::orm::config
