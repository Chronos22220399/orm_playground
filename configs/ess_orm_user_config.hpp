#pragma once
#include <ess/orm/common/meta.hpp>
#include <ess/orm/core/dialect.hpp>

struct MainDB {
  static constexpr std::string_view connection_url = "./data/test.db";
  static constexpr std::size_t pool_size = 50;
  static constexpr std::chrono::milliseconds busy_timeout{10000};
};

struct LoggerDB {
  static constexpr std::string_view connection_url =
      "file::memory:?cache=shared";
  static constexpr std::size_t pool_size = 4;
};

struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;

  using databases = std::tuple< //
      MainDB,                   //
      LoggerDB                  //
      >;

  using default_db = MainDB;

  static constexpr std::size_t default_container_size = 10;
};
