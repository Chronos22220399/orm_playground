#include <cstring>
#include <ess/orm/common/error.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/core/conn_factory.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/connection_pool.hpp>
#include <ess/orm/core/context.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/core/result.hpp>
#include <ess/orm/core/result_set_mapper.hpp>
#include <ess/orm/core/runtime.hpp>
#include <ess/orm/core/transaction.hpp>
#include <ess/orm/dsl/dsl.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parser.hpp>
#include <ess/orm/sql/validator.hpp>

using namespace std;
using namespace ess::orm;
using namespace ess::orm::meta;
using namespace ess::orm::core;
using namespace ess::orm::config;
using namespace ess::orm::dsl;
using namespace ess::orm::attribute;

enum class GoodsStatus : int { Normal = 0, Disabled, Deleted };
struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal; // enum
  bool enabled = true;

  using Database = default_db;
  using Schema = Schema<
      "goods", //
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<GoodsStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>> //
      >;
};

void test_default_cascade() {
  try {
    ess::orm::transaction([](auto &outer_tx) {
      std::cout << "--- 外层事务开始 ---" << std::endl;
      // 1. 外层做一次查询
      auto rows = outer_tx.template query<"SELECT * FROM goods">();
      std::cout << "外层查到商品数量: " << rows.size() << std::endl;

      // 2. 🌟 嵌套内层事务
      ess::orm::transaction([](auto &inner_tx) {
        std::cout << "  --- 内层事务开始 ---" << std::endl;
        // 故意在内层制造一个错误（比如抛出异常，或者执行非法的 SQL）
        std::cout << "  内层发生灾难，准备抛出异常..." << std::endl;
        auto res = inner_tx.template query<"DELETE FROM goods WHERE id = 1">();
        throw std::runtime_error("内层业务逻辑失败！");
        std::cout << "  --- 内层事务结束（这行不会被执行） ---" << std::endl;
      });

      rows = outer_tx.template query<"SELECT * FROM goods">();
      std::cout << "查到商品数量: " << rows.size() << std::endl;

      // 3. 外层后续代码
      std::cout << "外层后续业务...（因为内层报错，这行也不会被执行）"
                << std::endl;
    });
  } catch (int level) {
    // 按照你现在的设计，内层 level 抛出后，如果不加干涉，最终会在这里被捕获
    std::cout << ">>> 最终捕获：内层 Savepoint (" << level
              << ") 触发了回滚，外层被迫连坐！" << std::endl;
  } catch (const std::exception &e) {
    std::cout << ">>> 最终捕获到了其他异常: " << e.what() << std::endl;
  }
}

int main() {
  using namespace ess::orm::sql;
  using namespace ess::orm::core;

  test_default_cascade();

  // ess::orm::transaction([](auto &tx) {
  //   vector<Row> rows = tx.template query<"SELECT * FROM goods"_sql>();
  //
  //   for (auto &row : rows) {
  //     cout << row.get_if<int>("id").value() << " "
  //          << row.get_if<std::string>("title").value() << endl;
  //   }
  // });

  // auto conn = Context::instance().conn_pool().acquire();

  // cout << res.affected_rows << endl;

  // [[clang::annotate("cached", "strategy: writeback")]] //
  // [[clang::annotate("log", "path: default")]]
  // auto rows = ess::orm::query<Goods, "SELECT * FROM goods WHERE id > 0",
  //                             std::deque, ContainerSize<1000>>();
  return 0;
}
