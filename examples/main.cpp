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
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;

  using Database = default_db;
  using Schema = dsl::Schema<
      "goods", Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<GoodsStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>>>;
};

int main() {

  Context::instance().init();

  query<"SELECT * FROM goods"_sql>();

  // 1. 基础查询
  query<Goods, "SELECT * FROM goods"_sql>();
  query<Goods, "SELECT id, title FROM goods"_sql>();
  query<Goods, "SELECT * FROM goods WHERE id > 0"_sql>();
  query<Goods, "SELECT * FROM goods WHERE id = ?"_sql>(0);

  // 2. WHERE 条件
  query<Goods, "SELECT * FROM goods WHERE id IS NULL"_sql>();
  query<Goods, "SELECT * FROM goods WHERE id IS NOT NULL"_sql>();
  query<Goods, "SELECT * FROM goods WHERE id NOT BETWEEN 1 AND 10"_sql>();
  query<Goods, "SELECT * FROM goods WHERE title LIKE 'abc%'"_sql>();
  query<Goods, "SELECT * FROM goods WHERE id IN (1, 2, 3)"_sql>();
  query<Goods, "SELECT * FROM goods WHERE id IN (?, ?, ?)"_sql>(1, 2, 3);
  query<Goods, "SELECT * FROM goods WHERE id IN (SELECT id FROM goods)"_sql>();
  query<Goods, "SELECT * FROM goods WHERE NOT (id = 1)"_sql>();
  query<Goods, "SELECT * FROM goods WHERE EXISTS (SELECT 1)"_sql>();

  // 3. 表别名与前缀
  query<Goods, "SELECT * FROM goods g"_sql>();
  query<Goods, "SELECT g.id FROM goods g WHERE g.id > 0"_sql>();
  query<Goods, "SELECT * FROM goods g ORDER BY g.id"_sql>();

  // 4. 表达式列（运行时需要对应列映射，编译期通过）
  query<Goods, "SELECT id + 1 FROM goods"_sql>();
  query<Goods, "SELECT id * 2 FROM goods"_sql>();
  query<Goods, "SELECT -id FROM goods"_sql>();
  query<Goods, "SELECT (id + 1) * 2 FROM goods"_sql>();
  query<Goods, "SELECT UPPER(title) FROM goods"_sql>();
  query<Goods, "SELECT LOWER(title), ABS(price) FROM goods"_sql>();
  query<Goods, "SELECT 123, 'text', TRUE, FALSE, NULL FROM goods"_sql>();
  query<Goods, "SELECT ? FROM goods"_sql>("test");
  query<Goods, "SELECT id, id + 1, price * 2 FROM goods"_sql>();
  query<Goods, "SELECT g.id + g.stock FROM goods g"_sql>();

  // 5. LIMIT / OFFSET
  query<Goods, "SELECT * FROM goods LIMIT 10"_sql>();
  query<Goods, "SELECT * FROM goods LIMIT 10 OFFSET 20"_sql>();

  // 6. 聚合函数
  query<Goods, "SELECT COUNT(*) FROM goods"_sql>();
  query<Goods,
        "SELECT COUNT(id), SUM(price), AVG(price), MAX(price), MIN(price) FROM "
        "goods "_sql>();

  // 7. GROUP BY / HAVING
  query<Goods, "SELECT status, COUNT(*) FROM goods GROUP BY "
               "status "_sql>();
  query<Goods, "SELECT id % 10, "
               "COUNT(*) FROM goods GROUP BY id % 10 "_sql>();
  query<
      Goods,
      "SELECT price * stock, SUM(stock) FROM goods GROUP BY price * stock "_sql>();
  query<
      Goods,
      "SELECT UPPER(title), COUNT(*) FROM goods GROUP BY UPPER(title) "_sql>();
  query<
      Goods,
      "SELECT status, COUNT(*) FROM goods GROUP BY status HAVING COUNT(*) > 0 "_sql>();

  // 8. ORDER BY
  query<Goods, "SELECT * FROM goods ORDER BY id"_sql>();
  query<Goods, "SELECT * FROM goods ORDER BY id DESC"_sql>();
  query<Goods, "SELECT * FROM goods ORDER BY status, price * stock"_sql>();
  query<Goods, "SELECT * FROM goods ORDER BY id + 1"_sql>();
  query<Goods, "SELECT * FROM goods ORDER BY UPPER(title)"_sql>();
  query<Goods, "SELECT * FROM goods ORDER BY (id + 1) / 2"_sql>();
  // query<
  //     Goods,
  //     "SELECT status, COUNT(*) FROM goods GROUP BY status ORDER BY COUNT(*)
  //     DESC"_sql>();

  // 9. JOIN
  query<Goods,
        "SELECT * FROM goods JOIN category ON goods.cid = category.id"_sql>();

  // 10. FROM 子查询
  query<Goods, "SELECT * FROM (SELECT * FROM goods) t"_sql>();

  // 11. 复合查询
  query<
      Goods,
      "SELECT * FROM goods WHERE id > 0 UNION SELECT * FROM goods WHERE id < 100"_sql>();
  query<
      Goods,
      "SELECT * FROM goods WHERE id > 0 UNION ALL SELECT * FROM goods WHERE id < 100"_sql>();
  query<
      Goods,
      "SELECT * FROM goods WHERE id > 0 INTERSECT SELECT * FROM goods WHERE id < 100"_sql>();
  query<
      Goods,
      "SELECT * FROM goods WHERE id > 0 EXCEPT SELECT * FROM goods WHERE id < 100"_sql>();

  // 12. WITH 子句 (CTE)
  query<Goods, "WITH cte AS (SELECT * FROM goods) SELECT * FROM cte"_sql>();
  query<
      Goods,
      "WITH cte(id, title) AS (SELECT id, title FROM goods) SELECT * FROM cte"_sql>();
  query<
      Goods,
      "WITH cte1 AS (SELECT * FROM goods), cte2 AS (SELECT * FROM goods) SELECT * FROM cte1"_sql>();
  query<
      Goods,
      "WITH filtered AS (SELECT * FROM goods WHERE id > 0) SELECT * FROM filtered UNION SELECT * FROM goods"_sql>();
  query<
      Goods,
      "WITH cte1 AS (SELECT * FROM goods WHERE id > 0), cte2 AS (SELECT * FROM goods WHERE id < 100) SELECT * FROM cte1 UNION SELECT * FROM cte2"_sql>();

  // 13. INSERT 语法测试
  query<Goods, "INSERT INTO goods (title, price) VALUES ('test', 10.0)"_sql>();
  query<Goods, "INSERT INTO goods VALUES (1, 'test', 10.0, 100)"_sql>();
  query<Goods,
        "INSERT INTO goods (title, price) VALUES ('a', 1), ('b', 2)"_sql>();
  query<Goods, "INSERT INTO goods SELECT * FROM goods WHERE id > 0"_sql>();
  query<Goods, "INSERT INTO goods (title, price) VALUES (?, ?)"_sql>("test",
                                                                     10.1);
  query<Goods, "INSERT INTO goods (title, price) VALUES (DEFAULT, 10.0)"_sql>();
  query<Goods,
        "INSERT INTO goods VALUES (DEFAULT, 'test', DEFAULT, 100)"_sql>();
  // 表达式值和函数调用测试
  query<Goods, "INSERT INTO goods (price) VALUES (10.0 * 1.1)"_sql>();
  query<Goods, "INSERT INTO goods (id) VALUES (1 + 1)"_sql>();
  query<Goods, "INSERT INTO goods (title) VALUES (UPPER('test'))"_sql>();
  query<
      Goods,
      "INSERT INTO goods VALUES ((SELECT MAX(id) FROM goods), 'test', 10.0, 100)"_sql>();

  // 15. UPDATE 语法测试
  query<Goods, "UPDATE goods SET price = 10.0"_sql>();
  query<Goods, "UPDATE goods SET price = 10.0 WHERE id = 1"_sql>();
  query<Goods, "UPDATE goods SET title = 'new', price = 20.0"_sql>();
  query<Goods, "UPDATE goods SET price = ? WHERE id = ?"_sql>(99.9, 1);
  query<Goods, "UPDATE goods SET price = DEFAULT"_sql>();
  // 表达式值测试
  query<Goods, "UPDATE goods SET price = price * 1.1"_sql>();
  query<Goods, "UPDATE goods SET stock = stock + 1"_sql>();
  query<Goods, "UPDATE goods SET title = UPPER(title)"_sql>();
  query<Goods, "UPDATE goods SET price = (SELECT AVG(price) FROM goods)"_sql>();
  query<Goods, "UPDATE goods SET price = -price WHERE id > 0"_sql>();
  query<Goods, "UPDATE goods SET price = 10.0 * 2 + 5"_sql>();

  // 17. DELETE 语法测试
  query<Goods, "DELETE FROM goods"_sql>();
  query<Goods, "DELETE FROM goods WHERE id = 1"_sql>();
  query<Goods, "DELETE FROM goods WHERE id = ?"_sql>(1);
  query<Goods, "DELETE FROM goods WHERE id > 0 AND price < 100"_sql>();
  query<Goods, "DELETE FROM goods WHERE title LIKE '%old%'"_sql>();
  query<Goods, "DELETE FROM goods WHERE id IN (1, 2, 3)"_sql>();
  query<
      Goods,
      "DELETE FROM goods WHERE id IN (SELECT id FROM goods WHERE price > 100)"_sql>();
  query<Goods, "DELETE FROM goods WHERE status = ? AND enabled = ?"_sql>(
      "active", true);
  query<Goods, "DELETE FROM goods WHERE NOT (id = 1)"_sql>();
  query<
      Goods,
      "DELETE FROM goods WHERE EXISTS (SELECT 1 FROM goods WHERE id > 0)"_sql>();

  // 18. 错误检测测试（编译期应失败）
  // query<Goods, "SELECT * FROM goods WHERE id = NULL"_sql>();
  // query<Goods, "SELECT * FROM goods WHERE 1 IS NULL"_sql>();
  // query<Goods, "SELECT * FROM goods WHERE NULL IS NULL"_sql>();

  cout << "All SQL parser tests passed!" << endl;
  return 0;
}
