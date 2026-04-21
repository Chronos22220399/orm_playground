#pragma once
#include <ess/orm/common/meta.hpp>
#include <ess/orm/core/dialect.hpp>

struct TestDB {
  // 使用内存数据库，避免文件系统依赖
  static constexpr std::string_view connection_url = ":memory:";
  // sqlite3 选填，mysql、postgres 必填
  static constexpr std::string_view password = "";
  // 选填
  static constexpr std::size_t pool_size = 5;
  static constexpr auto query_timeout = std::chrono::milliseconds{3000};
  static constexpr auto busy_timeout = std::chrono::milliseconds{30000};
  static constexpr bool enable_sql_logging = false;
  static constexpr bool enable_wal_mode = true;
};

struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;

  using databases = std::tuple<TestDB>;

  using default_db = TestDB;

  static constexpr std::size_t default_container_size = 100;
};