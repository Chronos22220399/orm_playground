#pragma once
#include <ess/orm/dialect.hpp>

struct UserConfig {
  using dialect = ess::orm::dialect::Postgres;
};
