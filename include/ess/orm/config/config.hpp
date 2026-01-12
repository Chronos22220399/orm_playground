#pragma once
#include <ess/orm/config/traits.hpp>

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

static constexpr bool enable_sql_logging = config::enable_sql_logging;

static constexpr std::size_t pool_size = config::pool_size;

static constexpr std::chrono::milliseconds query_timeout =
    config::query_timeout;

} // namespace ess::orm::config
