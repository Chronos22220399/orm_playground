#include <chrono>
#include <concepts>
#include <ess/orm/config/default.hpp>
#include <ess/orm/dialect.hpp>

namespace ess::orm::config {
namespace detail {

template <typename T>
concept has_dialect = requires {
  typename T::dialect;
  requires dialect::dialect_type<typename T::dialect>;
};

template <typename T, typename Default> struct get_dialect_or {
  using type = Default;
};

template <typename T, typename Default>
  requires(has_dialect<T>)
struct get_dialect_or<T, Default> {
  using type = T::dialect;
};

template <typename T>
concept has_connection_url = requires {
  { T::connection_url } -> std::convertible_to<std::string>;
};

template <typename T>
concept has_password = requires {
  { T::password } -> std::convertible_to<std::string>;
};

template <typename T>
concept has_enable_sql_logging = requires {
  { T::enable_sql_logging } -> std::convertible_to<bool>;
};

template <typename T>
concept has_pool_size = requires {
  { T::pool_size } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept has_query_timeout = requires {
  { T::query_timeout } -> std::convertible_to<std::chrono::milliseconds>;
};

} // namespace detail

template <typename UserCfg, typename DefaultCfg = config::DefaultConfig>
struct ConfigTrait {
  // dialect
  using dialect =
      detail::get_dialect_or<UserCfg, typename DefaultCfg::dialect>::type;

  // connection_url
  static constexpr std::string connection_url = []() {
    if constexpr (detail::has_connection_url<UserCfg>) {
      return UserCfg::connection_url;
    } else {
      return DefaultConfig::connection_url;
    }
  }();

  // password
  static constexpr std::string password = []() {
    if constexpr (detail::has_password<UserCfg>) {
      return UserCfg::password;
    } else {
      return DefaultConfig::password;
    }
  }();

  // enable_sql_logging
  static constexpr bool enable_sql_logging = []() {
    if constexpr (detail::has_enable_sql_logging<UserCfg>) {
      return UserCfg::enable_sql_logging;
    } else {
      return DefaultConfig::enable_sql_logging;
    }
  }();

  // pool size
  static constexpr std::size_t pool_size = []() {
    if constexpr (detail::has_pool_size<UserCfg>) {
      return UserCfg::pool_size;
    } else {
      return DefaultConfig::pool_size;
    }
  }();

  // query timeout
  static constexpr std::chrono::milliseconds query_timeout = []() {
    if constexpr (detail::has_query_timeout<UserCfg>) {
      return UserCfg::query_timeout;
    } else {
      return DefaultCfg::query_timeout;
    }
  }();
};
} // namespace ess::orm::config
