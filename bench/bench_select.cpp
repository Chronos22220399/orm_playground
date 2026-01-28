#include <benchmark/benchmark.h>
#include "bench_main.cpp"
#include "common.hpp"

using namespace bench;
using namespace ess::orm;
using namespace ess::orm::config;

class SelectFixture : public benchmark::Fixture {
public:
  static NativeSqlite *native_db;
  static bool orm_data_ready;
  static std::vector<int> random_ids;
  static std::size_t id_index;
  static constexpr int RECORD_COUNT = 10000;

  void SetUp(const benchmark::State &) override {
    if (!native_db) {
      native_db = new NativeSqlite();
      DataGenerator gen;
      auto data = gen.generate(RECORD_COUNT);
      native_db->insert_batch(data);

      random_ids.reserve(1000);
      for (int i = 0; i < 1000; ++i) {
        random_ids.push_back(gen.random_id(RECORD_COUNT));
      }
    }

    if (!orm_data_ready) {
      ensure_orm_init();

      DataGenerator gen;
      auto data = gen.generate(RECORD_COUNT);

      transaction<Write, TestDB>([](auto &tx) {
        tx.template query<orm_entity::Goods, "DELETE FROM goods">();
      });

      transaction<Write, TestDB>([&data](auto &tx) {
        for (const auto &g : data) {
          tx.template query<orm_entity::Goods,
                            "INSERT INTO goods VALUES (?, ?, ?, ?, ?, ?)">(
              g.id, g.title, g.price, g.stock, g.status, g.enabled);
        }
      });

      orm_data_ready = true;
    }
    id_index = 0;
  }

  int next_id() { return random_ids[id_index++ % random_ids.size()]; }
};

NativeSqlite *SelectFixture::native_db = nullptr;
bool SelectFixture::orm_data_ready = false;
std::vector<int> SelectFixture::random_ids;
std::size_t SelectFixture::id_index = 0;

// ============ 原生 SQLite ============

BENCHMARK_DEFINE_F(SelectFixture, Native_SelectAll)(benchmark::State &state) {
  for (auto _ : state) {
    auto result = native_db->select_all();
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(state.iterations() * RECORD_COUNT);
}
BENCHMARK_REGISTER_F(SelectFixture, Native_SelectAll);

BENCHMARK_DEFINE_F(SelectFixture, Native_SelectOne)(benchmark::State &state) {
  for (auto _ : state) {
    auto result = native_db->select_one(next_id());
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(SelectFixture, Native_SelectOne);

BENCHMARK_DEFINE_F(SelectFixture, Native_Count)(benchmark::State &state) {
  for (auto _ : state) {
    int count = native_db->count();
    benchmark::DoNotOptimize(count);
  }
}
BENCHMARK_REGISTER_F(SelectFixture, Native_Count);

// ============ ORM ============

BENCHMARK_DEFINE_F(SelectFixture, ORM_SelectAll)(benchmark::State &state) {
  for (auto _ : state) {
    // 使用 Read 事务
    auto result = transaction<Read, TestDB>([](auto &tx) {
      return tx.template query<orm_entity::Goods, "SELECT * FROM goods">();
    });
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(state.iterations() * RECORD_COUNT);
}
BENCHMARK_REGISTER_F(SelectFixture, ORM_SelectAll);

BENCHMARK_DEFINE_F(SelectFixture, ORM_SelectOne)(benchmark::State &state) {
  for (auto _ : state) {
    auto result = transaction<Read, TestDB>([this](auto &tx) {
      return tx.template query<orm_entity::Goods,
                               "SELECT * FROM goods WHERE id = ?">(next_id());
    });
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(SelectFixture, ORM_SelectOne);

// 使用外部 query（非事务）
BENCHMARK_DEFINE_F(SelectFixture, ORM_SelectAll_NoTx)(benchmark::State &state) {
  for (auto _ : state) {
    auto result = query<orm_entity::Goods, "SELECT * FROM goods">();
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(state.iterations() * RECORD_COUNT);
}
BENCHMARK_REGISTER_F(SelectFixture, ORM_SelectAll_NoTx);

BENCHMARK_DEFINE_F(SelectFixture, ORM_SelectOne_NoTx)(benchmark::State &state) {
  for (auto _ : state) {
    auto result =
        query<orm_entity::Goods, "SELECT * FROM goods WHERE id = ?">(next_id());
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(state.iterations());
}
BENCHMARK_REGISTER_F(SelectFixture, ORM_SelectOne_NoTx);
