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
                     "SELECT goods.id, COUNT(id) as cnt_id FROM goods "
                     "WHERE id IN ( "
                     "SELECT id FROM goods WHERE id "
                     "IS NOT NULL AND id > 0 "
                     "GROUP BY id "
                     "HAVING COUNT(id) > ?) "
                     "ORDER BY id ASC, title DESC "_sql>(0);

  auto goods2 =
      query<Goods, //
            "SELECT * FROM goods WHERE id NOT BETWEEN 1 AND 10"_sql>();

  auto goods3 = query<
      Goods, //
      "SELECT * FROM goods WHERE 11 - 1 = id OR 11 + 1 = id AND id = 1 + 1"_sql>();

  auto goods4 = query<Goods, //
                      "SELECT * FROM goods WHERE EXISTS (SELECT 1)"_sql>();

  auto goods5 = query<
      Goods, //
      "SELECT * FROM goods WHERE NOT EXISTS (SELECT * FROM goods WHERE id = TRUE)"_sql>();

  auto goods6 = query<Goods, //
                      "SELECT * FROM goods WHERE -id > 0"_sql>();

  auto goods7 = query<Goods, //
                      "SELECT * FROM goods WHERE id IS NULL"_sql>();

  auto goods8 = query<Goods, //
                      "SELECT * FROM goods WHERE id IS NOT NULL"_sql>();

  auto goods9 = query<Goods, //
                      "SELECT * FROM goods g"_sql>();

  auto goods10 = query<Goods, //
                       "SELECT g.id FROM goods g WHERE g.id > 0"_sql>();

  auto goods11 = query<Goods, //
                       "SELECT * FROM goods LIMIT 10"_sql>();

  auto goods12 =
      query<Goods, //
            "SELECT * FROM goods WHERE id = 10 LIMIT 10 OFFSET 20"_sql>();

  auto goods13 = query<Goods, //
                       "SELECT * FROM goods LIMIT 5 OFFSET 10"_sql>();

  auto goods14 = query<Goods, //
                       "SELECT * FROM (SELECT * FROM goods) t"_sql>();

  auto goods15 = query<
      Goods, //
      "SELECT * FROM goods, category WHERE goods.cid = category.id"_sql>();

  auto goods16 = query<
      Goods, //
      "SELECT * FROM goods JOIN category ON goods.cid = category.id"_sql>();

  auto goods17 = query<
      Goods, //
      "SELECT * FROM goods OUTER JOIN category ON goods.cid = category.id JOIN inventory ON inventory.id = goods.inv_id"_sql>();

  auto goods18 = query<Goods, "SELECT COUNT(DISTINCT id) FROM goods"_sql>();
  auto goods19 = query<Goods, "SELECT * FROM goods ORDER BY COUNT(id)"_sql>();

  std::cout << "商品数量: " << goods.size() << std::endl;
  std::cout << "商品数量2: " << goods2.size() << std::endl;
  std::cout << "商品数量3: " << goods3.size() << std::endl;
  std::cout << "商品数量4: " << goods4.size() << std::endl;
  std::cout << "商品数量5: " << goods5.size() << std::endl;
  std::cout << "商品数量6 (一元负号): " << goods6.size() << std::endl;
  std::cout << "商品数量7 (IS NULL): " << goods7.size() << std::endl;
  std::cout << "商品数量8 (IS NOT NULL): " << goods8.size() << std::endl;
  std::cout << "商品数量9 (表别名): " << goods9.size() << std::endl;
  std::cout << "商品数量10 (WHERE表别名): " << goods10.size() << std::endl;
  std::cout << "商品数量11 (LIMIT): " << goods11.size() << std::endl;
  std::cout << "商品数量12 (LIMIT+OFFSET): " << goods12.size() << std::endl;
  std::cout << "商品数量13 (LIMIT+OFFSET): " << goods13.size() << std::endl;
  std::cout << "商品数量14 (FROM子查询): " << goods14.size() << std::endl;
  std::cout << "商品数量15 (多表+WHERE): " << goods15.size() << std::endl;
  std::cout << "商品数量16 (JOIN): " << goods16.size() << std::endl;
  std::cout << "商品数量17 (LEFT JOIN): " << goods17.size() << std::endl;
  std::cout << "商品数量18 (COUNT DISTINCT): " << goods18.size() << std::endl;
  std::cout << "商品数量19 (ORDER BY aggregate): " << goods19.size()
            << std::endl;
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

  // ========== 以下是错误测试用例（编译失败）============

  // 错误1: col = NULL (应拦截)
  // auto bad1 = query<Goods, "SELECT * FROM goods WHERE id = NULL"_sql>();

  // 错误2: 1 IS NULL (无意义表达式)
  // auto bad2 = query<Goods, "SELECT * FROM goods WHERE 1 IS NULL"_sql>();

  // 错误3: NULL IS NULL (无意义表达式)
  // auto bad3 = query<Goods, "SELECT * FROM goods WHERE NULL IS NULL"_sql>();

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
