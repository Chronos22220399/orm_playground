#pragma once
#include <ess/orm/dialect.hpp>
#include <ess/orm/meta.hpp>

struct MainDB {
  static constexpr std::string connection_url = "./data/test.db";
};

struct LoggerDB {
  static constexpr std::string connection_url = "./data/test1.db";
};

struct UserConfig {
  using dialect = ess::orm::dialect::Postgres;

  using databases = std::tuple< //
      MainDB,                   //
      LoggerDB                  //
      >;

  using default_db = MainDB;
};
