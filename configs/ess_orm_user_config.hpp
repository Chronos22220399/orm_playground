#pragma once
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/meta.hpp>

struct MainDB {
  static constexpr std::string_view connection_url = "./data/test.db";
};

struct LoggerDB {
  static constexpr std::string_view connection_url = "./data/test1.db";
  static constexpr std::size_t pool_size = 4;
};

struct TestDB {
  static constexpr std::string_view connection_url = "./bench/data/test_orm.db";
  static constexpr std::size_t pool_size = 3;
};

struct UserConfig {
  using dialect = ess::orm::dialect::Sqlite3;

  using databases = std::tuple< //
      MainDB,                   //
      LoggerDB,                 //
      TestDB                    //
      >;

  using default_db = MainDB;
};
