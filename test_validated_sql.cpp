#include <ess/orm/sql/validator.hpp>
#include <iostream>

using namespace ess::orm::sql;

// 测试SqlParseResult类型
void test_validated_sql() {
  std::cout << "Testing SqlParseResult type...\n";

  // 测试1：基础语法验证
  {
    constexpr auto validated = "SELECT * FROM users"_sql;
    std::cout << "Test 1 passed: SqlParseResult created\n";
    std::cout << "  SQL: " << std::string_view(validated.str()) << "\n";
    std::cout << "  Placeholder count: " << validated.placeholder_count()
              << "\n";
  }

  // 测试2：带占位符的SQL
  {
    constexpr auto validated = "SELECT * FROM users WHERE id = ?"_sql;
    std::cout << "Test 2 passed: SQL with placeholder\n";
    std::cout << "  SQL: " << std::string_view(validated.str()) << "\n";
    std::cout << "  Placeholder count: " << validated.placeholder_count()
              << "\n";
  }

  // 测试3：多个占位符
  {
    constexpr auto validated =
        "SELECT * FROM users WHERE id = ? AND name = ?"_sql;
    std::cout << "Test 3 passed: SQL with multiple placeholders\n";
    std::cout << "  SQL: " << std::string_view(validated.str()) << "\n";
    std::cout << "  Placeholder count: " << validated.placeholder_count()
              << "\n";
  }

  // 测试4：算术表达式（应该能工作）
  {
    constexpr auto validated = "SELECT id + 1 FROM goods"_sql;
    std::cout << "Test 4 passed: SQL with arithmetic expression\n";
    std::cout << "  SQL: " << std::string_view(validated.str()) << "\n";
  }

  // 测试5：无效SQL（应该触发编译错误）
  // 取消注释以下代码应该导致编译错误
  /*
  {
      constexpr auto validated = "SELECT FROM"_sql; // 无效SQL
      std::cout << "Test 5: This should not compile\n";
  }
  */

  std::cout << "\nAll SqlParseResult tests passed!\n";
}

int main() {
  try {
    test_validated_sql();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}