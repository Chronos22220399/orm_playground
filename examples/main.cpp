#include <ess/orm/orm.hpp>

using namespace std;
using namespace ess::orm;
using namespace ess::orm::sql;
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
  using Schema = dsl::Schema<
      "goods", //
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<GoodsStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>> //
      >;
};

void test_default_no_cascade();

int main() {

  Context::instance().init();
  // Context::instance().register_db<default_db>();

  auto goods = query<Goods, //
                     "SELECT * FROM goods "
                     "WHERE id IN ( "
                     "SELECT id FROM goods WHERE id "
                     "IS NOT NULL AND id > 0 "
                     "GROUP BY id "
                     "HAVING COUNT(id) > ?) "
                     "ORDER BY id ASC, title DESC "_sql>(0);

  std::cout << "商品数量: " << goods.size() << std::endl;
  std::cout << "商品信息: " << std::endl;
  for (auto &g : goods) {
    cout << g.id << " " << g.title << " " << g.price << " " << endl;
  }

  // test_default_no_cascade();

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

void test_default_no_cascade() {
  using namespace ess::orm::sql;

  ess::orm::transaction([](auto &outer_tx) {
    std::cout << "--- 外层事务开始 ---" << std::endl;

    auto rows = outer_tx.template query< //
        "SELECT * FROM goods WHERE id IN"
        "(SELECT id FROM goods WHERE id > ?)"_sql //
        >(0);

    std::cout << "外层查到商品数量: " << rows.size() << std::endl;

    ess::orm::transaction([](auto &inner_tx) {
      std::cout << "  --- 内层事务开始 ---" << std::endl;
      std::cout << "  内层发生灾难，准备抛出异常..." << std::endl;
      auto res = inner_tx.template query<"DELETE FROM goods WHERE id = 1">();
      throw std::runtime_error("内层业务逻辑失败！");
      std::cout << "  --- 内层事务结束（这行不会被执行） ---" << std::endl;
    });

    rows = outer_tx.template query<"SELECT * FROM goods">();
    std::cout << "查到商品数量: " << rows.size() << std::endl;
    std::cout << "外层后续业务...（因为内层报错，这行也不会被执行）"
              << std::endl;
  });
}
