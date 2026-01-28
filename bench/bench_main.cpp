// bench/bench_main.cpp
#include <benchmark/benchmark.h>

// 你的实际头文件路径
#include <ess/orm/config/config.hpp>
#include <ess/orm/context.hpp> // 或 context.hpp
#include <ess/orm/dsl.hpp>
#include <ess/orm/runtime.hpp>
#include <ess/orm/transaction.hpp>

// 用户配置
#include <ess_orm_user_config.hpp>

using namespace ess::orm;
namespace orm_entity {

enum class GoodsStatus { Normal = 0, Disabled, Deleted };

struct Goods {
  int id = 0;
  std::string title;
  double price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;

  using Database = config::TestDB;

  using Schema =
      ess::orm::dsl::Schema<"goods", ess::orm::dsl::Field<"id", &Goods::id>,
                            ess::orm::dsl::Field<"title", &Goods::title>,
                            ess::orm::dsl::Field<"price", &Goods::price>,
                            ess::orm::dsl::Field<"stock", &Goods::stock>,
                            ess::orm::dsl::Field<"status", &Goods::status>,
                            ess::orm::dsl::Field<"enabled", &Goods::enabled>>;
};

} // namespace orm_entity

inline bool g_orm_initialized = false;

inline void ensure_orm_init() {
  if (!g_orm_initialized) {
    ess::orm::Context::instance().init();

    auto conn =
        ess::orm::Context::instance().conn_pool<config::TestDB>().acquire();
    conn->execute_raw(R"(
                      DROP TABLE IF EXISTS goods
                      )");
    conn->execute_raw(R"(
            CREATE TABLE IF NOT EXISTS goods (
                id INTEGER PRIMARY KEY,
                title TEXT NOT NULL,
                price REAL,
                stock INTEGER,
                status INTEGER,
                enabled INTEGER
            )
        )");

    g_orm_initialized = true;
  }
}
