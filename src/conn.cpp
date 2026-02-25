#include <cstring>
#include <ess/orm/config/config.hpp>
#include <ess/orm/context.hpp>
#include <ess/orm/core/conn_factory.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/connection_pool.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/dsl.hpp>
#include <ess/orm/error.hpp>
#include <iostream>
#include <sqlite3.h>
#include <thread>

using namespace ess::orm;
using namespace ess::orm::core;

struct Log {
  int id;

  using Database = config::LoggerDB;
  using Schema = dsl::Schema<"log", dsl::Field<"id", &Log::id>>;
};

int main() {
  std::vector<std::thread> threads;
  // auto pool = ConnectionPool<dialect::Sqlite3>::create("./data/test.db", 8);
  // std::atomic_int err_cnt = 0;
  // for (int i = 0; i < 3; ++i) {
  //   threads.emplace_back([&pool, &err_cnt]() {
  //     auto conn = pool->acquire();
  //     try {
  //       conn->begin_transaction(TxMode::WRITE); // 外层
  //       conn->execute_raw("SELECT * FROM goods");
  //       conn->begin_transaction(TxMode::WRITE); // 内层 SAVEPOINT
  //       auto &stmt = conn->prepare_cached(
  //           "UPDATE goods SET stock = stock + 1 WHERE id > 1");
  //       stmt.next();
  //       std::cout << conn->affected_rows() << std::endl;
  //       conn->commit();
  //       conn->commit();
  //     } catch (const std::exception &e) {
  //       err_cnt++;
  //       while (conn->nesting_level() > 0)
  //         conn->rollback();
  //       std::cout << e.what() << std::endl;
  //     }
  //   });
  // }
  auto &pool = Context::instance().conn_pool<config::LoggerDB>();
  auto conn = pool.acquire();
  auto &stmt = conn->prepare_cached("SELECT * FROM log");
  stmt.next();
  std::cout << stmt.column_count() << std::endl;

  for (auto &t : threads)
    t.join();

  // std::cout << err_cnt << std::endl;
  return 0;
}
