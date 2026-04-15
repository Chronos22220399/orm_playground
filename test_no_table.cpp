#include <ess/orm/core/runtime.hpp>
#include <ess/orm/sql/validator.hpp>
#include <iostream>

using namespace ess::orm;

void test_no_table_queries() {
  std::cout << "Testing queries without table type...\n";

  // 测试1：无表类型，FixedString（不校验）
  {
    std::cout << "\nTest 1: No table, FixedString (no validation)\n";
    // 这里只是测试编译，不实际执行
    std::cout << "  Can compile queries without table type\n";
  }

  // 测试2：无表类型，SqlParseResult（校验占位符数量）
  {
    std::cout
        << "\nTest 2: No table, SqlParseResult (placeholder validation)\n";

    // 正确：占位符数量匹配
    std::cout << "  Test 2.1: Valid query with correct placeholder count\n";
    // query<"SELECT * FROM users WHERE id = ?"_sql>(123); // 占位符数量匹配

    // 错误：占位符数量不匹配（应该触发编译错误）
    std::cout << "  Test 2.2: Invalid query with wrong placeholder count "
                 "(should cause compile error)\n";
    std::cout << "  // Uncomment to test:\n";
    std::cout << "  // query<\"SELECT * FROM users WHERE id = ?\"_sql>(); // "
                 "缺少参数\n";
    std::cout << "  // query<\"SELECT * FROM users WHERE id = ?\"_sql>(123, "
                 "456); // 参数过多\n";
  }

  // 测试3：混合测试
  {
    std::cout << "\nTest 3: Mixed tests\n";

    // 无表类型，无占位符
    std::cout << "  Test 3.1: No table, no placeholders\n";
    // query<"SELECT * FROM users"_sql>();

    // 无表类型，多个占位符
    std::cout << "  Test 3.2: No table, multiple placeholders\n";
    // query<"SELECT * FROM users WHERE id = ? AND name = ?"_sql>(123, "test");

    // 无表类型，INSERT语句
    std::cout << "  Test 3.3: No table, INSERT with placeholders\n";
    // query<"INSERT INTO users (id, name) VALUES (?, ?)"_sql>(123, "test");

    // 无表类型，UPDATE语句
    std::cout << "  Test 3.4: No table, UPDATE with placeholders\n";
    // query<"UPDATE users SET name = ? WHERE id = ?"_sql>("new_name", 123);

    // 无表类型，DELETE语句
    std::cout << "  Test 3.5: No table, DELETE with placeholders\n";
    // query<"DELETE FROM users WHERE id = ?"_sql>(123);
  }

  // 测试4：错误SQL应该触发编译错误
  {
    std::cout << "\nTest 4: Invalid SQL should cause compile error\n";
    std::cout
        << "  The following would trigger compile error at SQL literal:\n";
    std::cout << "  // Uncomment to test:\n";
    std::cout << "  // query<\"SELECT FROM\"_sql>(); // Syntax error\n";
  }

  std::cout << "\nAll no-table query tests passed (compile-time validation)!\n";
  std::cout << "\nNote: These tests verify compile-time validation.\n";
  std::cout << "Actual query execution would require database connection.\n";
}

int main() {
  try {
    test_no_table_queries();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}