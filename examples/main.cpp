#include "./coder/coder.hpp"
#include <ess/orm/orm.hpp>
#include <iomanip>
#include <iostream>

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
  long long id{};
  std::string title{};
  float price{0.0};
  int stock{0};
  GoodsStatus status{GoodsStatus::Deleted};
  bool enabled{true};

  using Database = default_db;
  using Schema = dsl::Schema<
      "goods",
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement, SerializedName<"1">>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<GoodsStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>>>;
};

// 函数声明
void setup_test_tables();
void test_queries_without_table_type();
void test_queries_with_table_type();
void test_table_type_restrictions();
void test_dml_operations();
void test_visitor();

int main() {
  // Context::instance().init();
  Goods goods{};
  goods.id = 1000;
  auto js = Coder::to_json(goods);
  cout << js["1"] << endl;

  js["1"] = 0;
  cout << goods.id << endl;
  Coder::from_json(js, goods);
  cout << js["1"] << endl;

  // setup_test_tables();
  //
  //
  // // 查询并显示插入的数据
  // cout << "\n查询插入的测试数据:" << endl;
  // auto goods_data = query<Goods, "SELECT * FROM goods ORDER BY id"_sql>();
  // for (const auto &goods : goods_data) {
  //   cout << "  id: " << goods.id << ", title: " << goods.title
  //        << ", price: " << goods.price << ", stock: " << goods.stock
  //        << ", status: " << static_cast<int>(goods.status)
  //        << ", enabled: " << (goods.enabled ? "true" : "false") << endl;
  // }
  //
  // int id = 0;
  // cin >> id;
  // auto goods_rows =
  //     query<"SELECT * FROM goods WHERE id >= ? ORDER BY id"_sql>(id);
  // for (const auto &goods : goods_rows) {
  //   int id = goods["id"];
  //   string title = goods["title"];
  //   float price = goods["price"];
  //   float stock = goods["stock"];
  //   int status = goods["status"];
  //   bool enabled = goods["enabled"];
  //   cout << "  id: " << id << ", title: " << title << ", price: " << price
  //        << ", stock: " << stock << ", status: " << static_cast<int>(status)
  //        << ", enabled: " << (enabled ? "true" : "false") << endl;
  // }

  // cout << "\n=== 无表类型查询测试 ===" << endl;
  // test_queries_without_table_type();
  //
  // cout << "\n=== 带表类型查询测试 ===" << endl;
  // test_queries_with_table_type();
  //
  // cout << "\n=== 验证带表类型查询的限制 ===" << endl;
  // test_table_type_restrictions();
  //
  // cout << "\n=== DML操作测试 ===" << endl;
  // test_dml_operations();
  //
  // cout << "\n=== 测试完成 ===" << endl;
  // cout << "所有有效的SQL解析测试通过！" << endl;
  // cout << "注意：带表类型查询有严格限制，确保查询只返回表字段。" << endl;
  //
  return 0;
}

// // 函数定义
// void setup_test_tables() {
//   auto loan = Context::instance().conn_pool().acquire();
//   auto &conn = *loan.get();
//   conn.execute_raw("DROP TABLE IF EXISTS goods");
//   conn.execute_raw("DROP TABLE IF EXISTS category");
//   conn.execute_raw(
//       "CREATE TABLE goods(id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, "
//       "price REAL, stock INTEGER, status INTEGER, enabled BOOLEAN)");
//   conn.execute_raw("CREATE TABLE category(cid INTEGER, name TEXT)");
//   conn.execute_raw("INSERT OR IGNORE INTO category VALUES(1,
//   'electronics')"); conn.execute_raw("INSERT OR IGNORE INTO category
//   VALUES(2, 'books')");
//
//   // 插入测试数据
//   cout << "插入测试数据:" << endl;
//   query<
//       "INSERT INTO goods (title, price, stock, status, enabled) VALUES
//       ('商品1', 10.5, 100, 0, 1)"_sql>();
//   query<
//       "INSERT INTO goods (title, price, stock, status, enabled) VALUES
//       ('商品2', 25.0, 50, 0, 1)"_sql>();
//   query<
//       "INSERT INTO goods (title, price, stock, status, enabled) VALUES
//       ('商品3', 99.9, 20, 1, 1)"_sql>();
//   query<
//       "INSERT INTO goods (title, price, stock, status, enabled) VALUES
//       ('商品4', 15.0, 200, 0, 0)"_sql>();
//   query<
//       "INSERT INTO goods (title, price, stock, status, enabled) VALUES
//       ('商品5', 45.5, 75, 0, 1)"_sql>();
// }
//
// void test_queries_without_table_type() {
//   // 1. 基础查询（无表类型）
//   cout << "\n1. 基础查询（无表类型）:" << endl;
//   query<"SELECT * FROM goods"_sql>();
//   query<"SELECT id, title FROM goods"_sql>();
//   query<"SELECT * FROM goods WHERE id > 0"_sql>();
//   query<"SELECT * FROM goods WHERE id = ?"_sql>(0);
//
//   // 2. WHERE条件（无表类型）
//   cout << "\n2. WHERE条件（无表类型）:" << endl;
//   query<"SELECT * FROM goods WHERE id IS NULL"_sql>();
//   query<"SELECT * FROM goods WHERE id IS NOT NULL"_sql>();
//   query<"SELECT * FROM goods WHERE title LIKE 'abc%'"_sql>();
//   query<"SELECT * FROM goods WHERE id IN (1, 2, 3)"_sql>();
//   query<"SELECT * FROM goods WHERE id IN (?, ?, ?)"_sql>(1, 2, 3);
//
//   // 3. 复杂查询（无表类型）- 允许表达式、聚合函数等
//   cout << "\n3. 复杂查询（无表类型）:" << endl;
//   query<"SELECT id + 1 FROM goods"_sql>();
//   query<"SELECT UPPER(title) FROM goods"_sql>();
//   query<"SELECT COUNT(*) FROM goods"_sql>();
//   query<"SELECT 123, 'text' FROM goods"_sql>();
//   // 注意：JOIN查询在无表类型中允许，但这里不测试因为需要正确的表结构
//
//   // 4. 子查询和复合查询（无表类型）
//   cout << "\n4. 子查询和复合查询（无表类型）:" << endl;
//   query<"SELECT * FROM (SELECT id, title FROM goods) sub"_sql>();
//   query<"SELECT id FROM goods UNION SELECT cid FROM category"_sql>();
//   query<"WITH cte AS (SELECT id, title FROM goods) SELECT * FROM cte"_sql>();
// }
//
// void test_queries_with_table_type() {
//   // 5. 基础查询（带表类型）- 自动映射到Goods对象
//   cout << "\n5. 基础查询（带表类型）:" << endl;
//   query<Goods, "SELECT id FROM goods"_sql>();
//   query<Goods, "SELECT id, title FROM goods"_sql>();
//   query<Goods, "SELECT * FROM goods WHERE id > 0"_sql>();
//   query<Goods, "SELECT * FROM goods WHERE id = ?"_sql>(0);
//
//   // 6. WHERE条件（带表类型）
//   cout << "\n6. WHERE条件（带表类型）:" << endl;
//   query<Goods, "SELECT * FROM goods WHERE id IS NULL"_sql>();
//   query<Goods, "SELECT * FROM goods WHERE id IS NOT NULL"_sql>();
//   query<Goods, "SELECT * FROM goods WHERE title LIKE 'abc%'"_sql>();
//   query<Goods, "SELECT * FROM goods WHERE id IN (1, 2, 3)"_sql>();
//   query<Goods, "SELECT * FROM goods WHERE id IN (?, ?, ?)"_sql>(1, 2, 3);
//
//   // 7. 表别名（带表类型允许，因为列名不变）
//   cout << "\n7. 表别名（带表类型允许）:" << endl;
//   query<Goods, "SELECT * FROM goods g"_sql>();
//   query<Goods, "SELECT g.id FROM goods g WHERE g.id > 0"_sql>();
//
//   // 8. 嵌套查询（带表类型允许，如果只返回Goods表的列）
//   cout << "\n8. 嵌套查询（带表类型允许）:" << endl;
//   query<Goods, "SELECT * FROM goods WHERE id IN (SELECT id FROM
//   goods)"_sql>(); query<Goods, "SELECT * FROM (SELECT id, title FROM goods)
//   sub"_sql>();
// }
//
// void test_table_type_restrictions() {
//   // 注意：以下查询会编译失败，因为违反了带表类型查询的限制
//   // 取消注释任意一行都会导致编译错误
//
//   // 9.1 JOIN查询（带表类型禁止）- 编译期验证
//   // query<Goods, "SELECT * FROM goods JOIN category ON goods.cid =
//   // category.id"_sql>(); 编译错误：has_joins()返回true，验证失败
//
//   // 9.2 列别名（带表类型禁止）- 编译期验证
//   // query<Goods, "SELECT id AS user_id FROM goods"_sql>();
//   // 编译错误：has_column_aliases()返回true，验证失败
//
//   // 9.3 表达式列（带表类型禁止）- 编译期验证
//   // query<Goods, "SELECT id + 1 FROM goods"_sql>();
//   // query<Goods, "SELECT UPPER(title) FROM goods"_sql>();
//   // 编译错误：has_expressions()返回true，验证失败
//
//   // 9.4 聚合函数（带表类型禁止）- 编译期验证
//   // query<Goods, "SELECT COUNT(*) FROM goods"_sql>();
//   // query<Goods, "SELECT SUM(price) FROM goods"_sql>();
//   // 编译错误：has_aggregates()返回true，验证失败
//
//   // 9.5 字面量（带表类型禁止）- 运行时验证
//   // 注意：字面量检查暂时未实现，但运行时列映射会失败
//   // query<Goods, "SELECT 123 FROM goods"_sql>();
//   // query<Goods, "SELECT 'text' FROM goods"_sql>();
//   // 运行时错误：unknown column: "123" 或 "'text'"
//
//   // 9.6 验证通过：表别名允许（列名不变）
//   cout << "表别名验证通过（允许使用）" << endl;
//
//   // 9.7 验证通过：嵌套查询允许（如果只返回Goods表的列）
//   cout << "嵌套查询验证通过（允许使用）" << endl;
// }
//
// void test_dml_operations() {
//   // 先清理可能存在的测试数据
//   query<Goods, "DELETE FROM goods WHERE title LIKE 'test%'"_sql>();
//
//   // 10. INSERT操作（使用DEFAULT让数据库自动生成id）
//   cout << "\n10. INSERT操作:" << endl;
//   query<Goods, "INSERT INTO goods (title, price) VALUES
//   ('test', 10.0)"_sql>(); query<Goods, "INSERT INTO goods (title, price)
//   VALUES (?, ?)"_sql>("test2",
//                                                                      20.0);
//
//   // 11. UPDATE操作（更新刚插入的数据）
//   cout << "\n11. UPDATE操作:" << endl;
//   query<Goods, "UPDATE goods SET price = 15.0 WHERE title = 'test'"_sql>();
//   query<Goods, "UPDATE goods SET price = ? WHERE title = ?"_sql>(25.0,
//   "test2");
//
//   // 12. DELETE操作（删除测试数据）
//   cout << "\n12. DELETE操作:" << endl;
//   query<Goods, "DELETE FROM goods WHERE title = 'test'"_sql>();
//   query<Goods, "DELETE FROM goods WHERE title = ?"_sql>("test2");
// }
//
// void test_visitor() {
//   Goods goods;
//   goods.id = 10;
//
//   Visitor<Goods>{}.on(goods).foreach ([](auto info, auto &value) {
//     cout << info.column_name << endl;
//     if constexpr (std::is_enum_v<typename decltype(info)::member_type>) {
//       cout << static_cast<int>(value) << endl;
//     } else {
//       cout << value << endl;
//     }
//   });
//   cout << endl;
//
//   Visitor<Goods>::foreach ([](auto info) {
//     using Info = decltype(info);
//     cout << Info::column_name << ": " << Info::index << endl;
//   });
// }
