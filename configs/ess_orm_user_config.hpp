#pragma once
#include <ess/orm/dialect.hpp>
#include <ess/orm/meta.hpp>

struct UserConfig {
  using dialect = ess::orm::dialect::Postgres;

  static constexpr std::string connection_url = "./data/test.db";

  static constexpr std::string password = "xxx";
};

// struct MainDB {
//
//   static constexpr std::string connection_url = "./data/test.db";
//
//   static constexpr std::string password = "xxx";
// };
