#include <benchmark/benchmark.h>
#include "bench_main.cpp"
#include "common.hpp"

using namespace bench;
using namespace ess::orm;
using namespace ess::orm::config;

// ============ 空事务开销 ============

static void BM_Native_Transaction_Empty(benchmark::State &state) {
  NativeSqlite db;

  for (auto _ : state) {
    db.exec("BEGIN IMMEDIATE");
    db.exec("COMMIT");
  }
}
BENCHMARK(BM_Native_Transaction_Empty);

static void BM_ORM_Transaction_Empty(benchmark::State &state) {
  ensure_orm_init();

  for (auto _ : state) {
    transaction<Write, TestDB>([](auto &) {
      // 空事务
    });
  }
}
BENCHMARK(BM_ORM_Transaction_Empty);

static void BM_ORM_ReadTransaction_Empty(benchmark::State &state) {
  ensure_orm_init();

  for (auto _ : state) {
    transaction<Read, TestDB>([](auto &) {
      // 空读事务
    });
  }
}
BENCHMARK(BM_ORM_ReadTransaction_Empty);

// ============ 事务内多操作 ============

static void BM_Native_Transaction_10Ops(benchmark::State &state) {
  NativeSqlite db;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db.handle(), "SELECT 1", -1, &stmt, nullptr);

  for (auto _ : state) {
    db.exec("BEGIN IMMEDIATE");
    for (int i = 0; i < 10; ++i) {
      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }
    db.exec("COMMIT");
  }

  sqlite3_finalize(stmt);
  state.SetItemsProcessed(state.iterations() * 10);
}
BENCHMARK(BM_Native_Transaction_10Ops);

static void BM_ORM_Transaction_10Ops(benchmark::State &state) {
  ensure_orm_init();

  for (auto _ : state) {
    transaction<Read, TestDB>([](auto &tx) {
      for (int i = 0; i < 10; ++i) {
        auto r = tx.template query_scaler<orm_entity::Goods, "SELECT 1">(
            return_type<int>{});
        benchmark::DoNotOptimize(r);
      }
    });
  }
  state.SetItemsProcessed(state.iterations() * 10);
}
BENCHMARK(BM_ORM_Transaction_10Ops);

// ============ 嵌套事务 ============

static void BM_Native_NestedTransaction(benchmark::State &state) {
  NativeSqlite db;

  for (auto _ : state) {
    db.exec("BEGIN IMMEDIATE");
    db.exec("SAVEPOINT sp_1");
    db.exec("RELEASE SAVEPOINT sp_1");
    db.exec("COMMIT");
  }
}
BENCHMARK(BM_Native_NestedTransaction);

static void BM_ORM_NestedTransaction(benchmark::State &state) {
  ensure_orm_init();

  for (auto _ : state) {
    transaction<Write, TestDB>([](auto &) {
      transaction<Write, TestDB>([](auto &) {
        // 嵌套事务
      });
    });
  }
}
BENCHMARK(BM_ORM_NestedTransaction);

// ============ 更新操作 ============

static void BM_Native_Update(benchmark::State &state) {
  NativeSqlite db;
  DataGenerator gen;
  auto data = gen.generate(1000);
  db.insert_batch(data);

  for (auto _ : state) {
    db.update(gen.random_id(1000), "Updated Title");
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Native_Update);

static void BM_ORM_Update(benchmark::State &state) {
  ensure_orm_init();

  static bool prepared = false;
  static DataGenerator gen;

  if (!prepared) {
    transaction<Write, TestDB>([](auto &tx) {
      tx.template query<orm_entity::Goods, "DELETE FROM goods">();
    });

    auto data = gen.generate(1000);
    transaction<Write, TestDB>([&data](auto &tx) {
      for (const auto &g : data) {
        tx.template query<orm_entity::Goods,
                          "INSERT INTO goods VALUES (?, ?, ?, ?, ?, ?)">(
            g.id, g.title, g.price, g.stock, g.status, g.enabled);
      }
    });
    prepared = true;
  }

  for (auto _ : state) {
    transaction<Write, TestDB>([](auto &tx) {
      tx.template query<orm_entity::Goods,
                        "UPDATE goods SET title = ? WHERE id = ?">(
          "Updated Title", gen.random_id(1000));
    });
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ORM_Update);
