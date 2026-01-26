#include <chrono>
#include <concepts>
#include <ess/orm/common_concept.hpp>
#include <ess/orm/config/default.hpp>
#include <ess/orm/dialect.hpp>

namespace ess::orm::config {
namespace detail {

// ============== database 配置萃取 ==================
template <typename T>
concept has_connection_url = requires {
  { T::connection_url } -> std::convertible_to<std::string>;
};

template <typename T>
concept has_password = requires {
  { T::password } -> std::convertible_to<std::string>;
};

template <typename T>
concept has_pool_size = requires {
  { T::pool_size } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept has_query_timeout = requires {
  { T::query_timeout } -> std::convertible_to<std::chrono::milliseconds>;
};

template <typename T>
concept has_busy_timeout = requires {
  { T::busy_timeout } -> std::convertible_to<std::chrono::milliseconds>;
};

template <typename T>
concept has_enable_sql_logging = requires {
  { T::enable_sql_logging } -> std::convertible_to<bool>;
};

template <typename T>
concept has_enable_wal_mode = requires {
  { T::enable_wal_mode } -> std::convertible_to<bool>;
};

} // namespace detail

template <concepts::database_type UserDB, typename DefaultDatabase>
struct DatabaseTrait {

  // connection_url
  static constexpr std::string connection_url = UserDB::connection_url;

  // password
  static constexpr std::string password = []() {
    if constexpr (detail::has_password<UserDB>) {
      return UserDB::password;
    } else {
      return DefaultDatabase::password;
    }
  }();

  // pool size
  static constexpr std::size_t pool_size = []() {
    if constexpr (detail::has_pool_size<UserDB>) {
      return UserDB::pool_size;
    } else {
      return DefaultDatabase::pool_size;
    }
  }();

  // query timeout
  static constexpr std::chrono::milliseconds query_timeout = []() {
    if constexpr (detail::has_query_timeout<UserDB>) {
      return UserDB::query_timeout;
    } else {
      return DefaultDatabase::query_timeout;
    }
  }();

  // busy timeout
  static constexpr std::chrono::milliseconds busy_timeout = []() {
    if constexpr (detail::has_query_timeout<UserDB>) {
      return UserDB::busy_timeout;
    } else {
      return DefaultDatabase::busy_timeout;
    }
  }();

  // enable_sql_logging
  static constexpr bool enable_sql_logging = []() {
    if constexpr (detail::has_enable_sql_logging<UserDB>) {
      return UserDB::enable_sql_logging;
    } else {
      return DefaultDatabase::enable_sql_logging;
    }
  }();

  // enable_wal_mode
  static constexpr bool enable_wal_mode = []() {
    if constexpr (detail::has_enable_sql_logging<UserDB>) {
      return UserDB::enable_wal_mode;
    } else {
      return DefaultDatabase::enable_wal_mode;
    }
  }();
};

// ================= 全局配置获取 ====================
namespace detail {
template <typename T>
concept has_dialect = requires {
  typename T::dialect;
  requires dialect::dialect_type<typename T::dialect>;
};

template <typename T>
concept has_databases = requires { typename T::databases; };

template <typename T>
concept has_default_db = requires { typename T::default_db; };

template <typename T, typename Default> struct get_dialect_or {
  using type = Default;
};

template <typename T, typename Default>
  requires has_dialect<T>
struct get_dialect_or<T, Default> {
  using type = T::dialect;
};

template <typename T, typename Default> struct get_databases_or {
  using type = Default;
};

template <typename T, typename Default>
  requires has_databases<T>
struct get_databases_or<T, Default> {
  using type = T::databases;
};

template <typename T, typename Databases> struct resolve_default_db {
  using type = std::tuple_element_t<0, Databases>;
};

template <typename T> struct resolve_default_db<T, std::tuple<>> {
  using type = void;
};

template <typename T, typename Databases>
  requires has_default_db<T>
struct resolve_default_db<T, Databases> {
  using type = typename T::default_db;
};

} // namespace detail

template <typename UserConfig, typename DefaultConfig>
struct GlobalConfigTrait {
  using dialect =
      detail::get_dialect_or<UserConfig, typename DefaultConfig::dialect>::type;

  using databases =
      detail::get_databases_or<UserConfig,
                               typename DefaultConfig::databases>::type;

  static constexpr std::size_t database_count = std::tuple_size_v<databases>;

  using default_db =
      detail::resolve_default_db<UserConfig,
                                 typename DefaultConfig::default_db>::type;

  static_assert(
      database_count > 0,
      "At least one database required, add your "
      "database at UserConfig::databases in ess_orm_user_config.hpp ");
};

} // namespace ess::orm::config
