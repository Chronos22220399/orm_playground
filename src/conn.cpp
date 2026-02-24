#include <cstring>
#include <ess/orm/config/config.hpp>
#include <ess/orm/core/conn_factory.h>
#include <ess/orm/core/connection.h>
#include <ess/orm/core/connection_pool.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/error.hpp>
#include <iostream>
#include <sqlite3.h>

using namespace ess::orm;
using namespace ess::orm::core;

int main() {
  auto pool = ConnectionPool<dialect::Sqlite3>::create("./data/test.db", 10);
  auto conn = pool->acquire();

  return 0;
}
