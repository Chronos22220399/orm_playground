#include <bit>
#include <cstring>
#include <ess/orm/config/config.hpp>
#include <ess/orm/context.hpp>
#include <ess/orm/core/conn_factory.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/connection_pool.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/core/runtime.hpp>
#include <ess/orm/core/transaction.hpp>
#include <ess/orm/dsl.hpp>
#include <ess/orm/error.hpp>
#include <iostream>
#include <sqlite3.h>
#include <thread>

using namespace ess::orm;
using namespace ess::orm::meta;
using namespace ess::orm::core;

struct Log {
  int id;
  int cnt;

  using Database = config::LoggerDB;
  using Schema = dsl::Schema<"log", dsl::Field<"id", &Log::id>>;
};

int main() {
  fmt::println("{}", std::bit_cast<uintptr_t>(&Log::cnt));
  // ess::orm::transaction<Write, config::LoggerDB>(
  //     [](auto &tx) { tx.template query<config::LoggerDB, "SELECT"_fs>(); });
  // std::vector<std::thread> threads;
  // auto &pool = Context::instance().conn_pool<config::LoggerDB>();
  // auto conn = pool.acquire();
  // auto &stmt = conn->prepare_cached("SELECT * FROM log");
  // stmt.next();
  // std::cout << stmt.column_count() << std::endl;
  //
  // for (auto &t : threads)
  //   t.join();

  // std::cout << err_cnt << std::endl;
  return 0;
}
