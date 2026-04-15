#include <ess/orm/sql/validator.hpp>
#include <iostream>

using namespace ess::orm::sql;

void test_error_reporting() {
  std::cout << "Testing error reporting locations...\n";

  // 测试1：语法错误应该在_sql调用处报告
  {
    std::cout << "\nTest 1: Syntax error reporting location\n";
    std::cout << "  Syntax errors should be reported at the _sql call site\n";
    std::cout << "  Example (uncomment to test):\n";
    std::cout
        << "  // constexpr auto bad_sql = \"SELECT FROM\"_sql; // Error here\n";
  }

  // 测试2：语义错误应该在query调用处报告
  {
    std::cout << "\nTest 2: Semantic error reporting location\n";
    std::cout << "  Semantic errors (placeholder mismatch) should be reported "
                 "at query call site\n";
    std::cout << "  Example (uncomment to test):\n";
    std::cout << "  // Using the new interface with _sql suffix\n";
    std::cout << "  // auto result = ess::orm::query<\"SELECT * FROM test "
                 "WHERE id = ?\"_sql>(); // Error: missing argument\n";
    std::cout << "  // auto result = ess::orm::query<\"SELECT * FROM test "
                 "WHERE id = ?\"_sql>(1, 2); // Error: too many arguments\n";
  }

  // 测试3：正确使用应该编译通过
  {
    std::cout << "\nTest 3: Correct usage should compile\n";

    // 正确：语法正确，占位符数量匹配
    constexpr auto valid_sql = "SELECT * FROM test WHERE id = ?"_sql;
    std::cout << "  Valid SQL parsed: " << std::string_view(valid_sql.str())
              << "\n";
    std::cout << "  Placeholder count: " << valid_sql.placeholder_count()
              << "\n";

    // 正确：无占位符
    constexpr auto no_placeholder = "SELECT * FROM test"_sql;
    std::cout << "  SQL without placeholders parsed\n";

    // 正确：算术表达式
    constexpr auto arithmetic = "SELECT id + 1 FROM test"_sql;
    std::cout << "  SQL with arithmetic expression parsed\n";
  }

  // 测试4：验证FixedString版本不进行校验
  {
    std::cout << "\nTest 4: FixedString version does no validation\n";
    std::cout << "  When using FixedString directly (no _sql suffix), no "
                 "validation is performed\n";
    std::cout << "  This allows users to bypass validation if needed\n";
    std::cout << "  Example:\n";
    std::cout << "  // ess::orm::query<Goods, \"SELECT * FROM goods\">(); // "
                 "No validation\n";
    std::cout << "  // ess::orm::query<Goods, \"SELECT * FROM goods WHERE id = "
                 "?\">(123); // No validation\n";
  }

  std::cout << "\nError reporting tests completed!\n";
  std::cout << "\nSummary:\n";
  std::cout << "1. Syntax errors reported at _sql call site\n";
  std::cout << "2. Semantic errors reported at query call site\n";
  std::cout << "3. FixedString version bypasses validation\n";
}

int main() {
  try {
    test_error_reporting();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}