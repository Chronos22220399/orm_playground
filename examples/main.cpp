#include "./coder/coder.hpp"
#include <ess/orm/orm.hpp>
#include <iomanip>
#include <iostream>
#include <thread>

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

struct TestDB {
  static constexpr std::string_view connection_url =
      "file::memory:?cache=shared";
  static constexpr std::size_t pool_size = 4;
};

struct Test {
  long long id{};
  string content{};

  using Database = TestDB;
  using Schema = dsl::Schema<             //
      "test",                             //
      Field<"id", &Test::id, PrimaryKey>, //
      Field<"content", &Test::content>    //
      >;
};

int main() {
  Context::instance().register_db<TestDB>();
  auto ddl = Test::Schema::make_create_table_ddl();
  cout << ddl << endl;
  auto loan = Context::instance().conn_pool<TestDB>().acquire();
  loan->execute_raw(ddl);

  query<Test, "insert into test values(?, ?)", TestDB>(1, "hello");

  auto res = query<Test, "select * from test"_sql, TestDB>();
  for (auto &t : res) {
    cout << t.id << " " << t.content << endl;
  }

  // 初始化时注册 configs/ess_orm_user_config.hpp 下的数据库信息
  // 默认下query和 conn_pool 都获取default_db
  Context::instance().init();
  auto res_g = query<Goods, "select * from goods limit 10 offset 0"_sql>();
  for (auto &g : res_g) {
    cout << g.id << " " << g.title << endl;
  }

  transaction([](auto &tx) {
    vector<Goods> rows =
        tx.template query<Goods, "select * from goods where id > ?"_sql>(0);
    cout << "Row Cnt: " << rows.size() << endl;
  });

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
