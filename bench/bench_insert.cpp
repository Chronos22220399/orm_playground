#include <benchmark/benchmark.h>
#include "bench_main.cpp"
#include "common.hpp"

using namespace bench;
using namespace ess::orm;
using namespace ess::orm::config;

// 测试数据
static DataGenerator g_generator;
static std::vector<RawGoods> g_data_100 = g_generator.generate(100);
static std::vector<RawGoods> g_data_1000 = g_generator.generate(1000);

// ============ 原生 SQLite ============

static void BM_Native_InsertOne(benchmark::State &state) {
  NativeSqlite db;
  int id = 1;

  for (auto _ : state) {
    state.PauseTiming();
    db.clear();
    id = 1;
    state.ResumeTiming();

    RawGoods g{id++, "Test Product", 99.99, 100, 0, 1};
    db.insert(g);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Native_InsertOne);

static void BM_Native_InsertBatch_100(benchmark::State &state) {
  NativeSqlite db;

  for (auto _ : state) {
    state.PauseTiming();
    db.clear();
    state.ResumeTiming();

    db.insert_batch(g_data_100);
  }
  state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_Native_InsertBatch_100);

static void BM_Native_InsertBatch_1000(benchmark::State &state) {
  NativeSqlite db;

  for (auto _ : state) {
    state.PauseTiming();
    db.clear();
    state.ResumeTiming();

    db.insert_batch(g_data_1000);
  }
  state.SetItemsProcessed(state.iterations() * 1000);
}
BENCHMARK(BM_Native_InsertBatch_1000);

// ============ ORM ============

static void BM_ORM_InsertOne(benchmark::State &state) {
  ensure_orm_init();
  int id = 100000;

  for (auto _ : state) {
    state.PauseTiming();
    // 清理之前插入的数据
    transaction<Write, TestDB>([](auto &tx) {
      tx.template query<orm_entity::Goods,
                        "DELETE FROM goods WHERE id >= 100000">();
    });
    id = 100000;
    state.ResumeTiming();

    // 单条插入
    transaction<Write, TestDB>([&](auto &tx) {
      tx.template query<
          orm_entity::Goods,
          "INSERT INTO goods (id, title, price, stock, status, enabled) "
          "VALUES (?, ?, ?, ?, ?, ?)">(id++, "Test Product", 99.99, 100, 0, 1);
    });
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ORM_InsertOne);

static void BM_ORM_InsertBatch_100(benchmark::State &state) {
  ensure_orm_init();

  for (auto _ : state) {
    state.PauseTiming();
    transaction<Write, TestDB>([](auto &tx) {
      tx.template query<orm_entity::Goods, "DELETE FROM goods">();
    });
    state.ResumeTiming();

    transaction<Write, TestDB>([](auto &tx) {
      for (const auto &g : g_data_100) {
        tx.template query<orm_entity::Goods,
                          "INSERT INTO goods VALUES (?, ?, ?, ?, ?, ?)">(
            g.id, g.title, g.price, g.stock, g.status, g.enabled);
      }
    });
  }
  state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_ORM_InsertBatch_100);

static void BM_ORM_InsertBatch_1000(benchmark::State &state) {
  ensure_orm_init();

  for (auto _ : state) {
    state.PauseTiming();
    transaction<Write, TestDB>([](auto &tx) {
      tx.template query<orm_entity::Goods, "DELETE FROM goods">();
    });
    state.ResumeTiming();

    transaction<Write, TestDB>([](auto &tx) {
      for (const auto &g : g_data_1000) {
        tx.template query<orm_entity::Goods,
                          "INSERT INTO goods VALUES (?, ?, ?, ?, ?, ?)">(
            g.id, g.title, g.price, g.stock, g.status, g.enabled);
      }
    });
  }
  state.SetItemsProcessed(state.iterations() * 1000);
}
BENCHMARK(BM_ORM_InsertBatch_1000);
