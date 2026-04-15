#include <ess/orm/orm.hpp>
#include <iostream>

using namespace std;
using namespace ess::orm;
using namespace ess::orm::sql;
using namespace ess::orm::meta;
using namespace ess::orm::core;
using namespace ess::orm::config;
using namespace ess::orm::dsl;
using namespace ess::orm::attribute;

// 测试表定义
struct TestTable {
  long long id = 0;
  std::string name;
  int value = 0;
  bool active = true;

  using Database = default_db;
  using Schema =
      dsl::Schema<"test_table",
                  Field<"id", &TestTable::id, PrimaryKey, AutoIncrement>,
                  Field<"name", &TestTable::name, DefaultValue<"unknown"_fs>>,
                  Field<"value", &TestTable::value, DefaultValue<0>>,
                  Field<"active", &TestTable::active, DefaultValue<true>>>;
};

int main() {
  cout << "测试列存在性验证..." << endl;

  try {
    // 1. 有效的列名 - 应该编译通过
    cout << "1. 测试有效列名..." << endl;
    query<TestTable, "SELECT id, name FROM test_table"_sql>();
    cout << "  ✓ SELECT id, name 通过" << endl;

    query<TestTable, "SELECT * FROM test_table"_sql>();
    cout << "  ✓ SELECT * 通过" << endl;

    query<TestTable,
          "SELECT id, value, active FROM test_table WHERE id = ?"_sql>(1);
    cout << "  ✓ SELECT 带占位符 通过" << endl;

    // 2. 测试INSERT语句
    query<TestTable, "INSERT INTO test_table (name, value) VALUES (?, ?)"_sql>(
        "test", 100);
    cout << "  ✓ INSERT 通过" << endl;

    // 3. 测试UPDATE语句
    query<TestTable,
          "UPDATE test_table SET name = ?, value = ? WHERE id = ?"_sql>(
        "updated", 200, 1);
    cout << "  ✓ UPDATE 通过" << endl;

    cout << "\n所有有效列名测试通过！" << endl;

  } catch (const std::exception &e) {
    cout << "错误: " << e.what() << endl;
    return 1;
  }

  return 0;
}