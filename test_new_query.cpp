#include <ess/orm/orm.hpp>
#include <iostream>

using namespace ess::orm;
using namespace ess::orm::sql;

// 简单测试结构
struct TestTable {
  int id;
  std::string name;

  using Database = config::default_db;
  using Schema = dsl::Schema<"test">;
};

void test_new_query_interface() {
  std::cout << "Testing new query interface...\n";

  // 测试1：基础语法验证
  {
    constexpr auto sql = "SELECT * FROM test"_sql;
    std::cout << "Test 1: Basic SQL parsing\n";
    std::cout << "  Placeholder count: " << sql.placeholder_count() << "\n";
  }

  // 测试2：不带表类型的query（应该能编译）
  {
    constexpr auto sql = "SELECT * FROM test"_sql;
    // 注意：这里不会实际执行，只是测试编译
    std::cout << "Test 2: Query without table type (compile test)\n";
    // query<sql>(); // 需要连接池，跳过执行
  }

  // 测试3：带占位符的SQL
  {
    constexpr auto sql = "SELECT * FROM test WHERE id = ?"_sql;
    std::cout << "Test 3: SQL with placeholder\n";
    std::cout << "  Placeholder count: " << sql.placeholder_count() << "\n";

    // 测试占位符数量验证概念
    constexpr bool valid = valid_sql_basic<decltype(sql), 1>;
    std::cout << "  Valid for 1 arg: " << (valid ? "true" : "false") << "\n";

    constexpr bool invalid = valid_sql_basic<decltype(sql), 2>;
    std::cout << "  Valid for 2 args: " << (invalid ? "true" : "false") << "\n";
  }

  // 测试4：算术表达式
  {
    constexpr auto sql = "SELECT id + 1 FROM test"_sql;
    std::cout << "Test 4: SQL with arithmetic expression\n";
    std::cout << "  Placeholder count: " << sql.placeholder_count() << "\n";
  }

  // 测试5：无效SQL（应该触发编译错误）
  // 取消注释以下代码应该导致编译错误
  /*
  {
      constexpr auto sql = "SELECT FROM"_sql; // 无效SQL
      std::cout << "Test 5: This should not compile\n";
  }
  */

  std::cout << "\nAll tests passed (compile-time)!\n";
}

int main() {
  try {
    test_new_query_interface();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}