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
  cout << "测试简单列存在性验证..." << endl;

  try {
    // 使用FixedString版本（不校验）
    cout << "1. 测试FixedString版本（不校验）..." << endl;
    query<TestTable, "SELECT id, name FROM test_table">();
    cout << "  ✓ FixedString版本通过" << endl;

    // 使用_sql后缀版本（完整校验）
    cout << "2. 测试_sql后缀版本（完整校验）..." << endl;
    // 这里应该会触发编译错误，因为列存在性验证会失败
    // 但让我们先测试一个有效的查询
    query<TestTable, "SELECT * FROM test_table"_sql>();
    cout << "  ✓ SELECT * 通过" << endl;

  } catch (const std::exception &e) {
    cout << "错误: " << e.what() << endl;
    return 1;
  }

  return 0;
}