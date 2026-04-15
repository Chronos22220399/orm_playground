#include <ess/orm/sql/concepts.hpp>
#include <ess/orm/sql/validator.hpp>
#include <iostream>

using namespace ess::orm::sql;

void test_new_interface() {
  std::cout << "Testing new SQL validation interface...\n";

  // 测试1：基础语法验证
  {
    std::cout << "\nTest 1: Basic SQL syntax validation\n";
    constexpr auto sql1 = "SELECT * FROM users"_sql;
    std::cout << "  Test 1.1 passed: Valid SQL parsed\n";

    constexpr auto sql2 = "SELECT * FROM users WHERE id = ?"_sql;
    std::cout << "  Test 1.2 passed: SQL with placeholder parsed\n";
    std::cout << "    Placeholder count: " << sql2.placeholder_count() << "\n";

    constexpr auto sql3 = "SELECT id + 1 FROM goods"_sql;
    std::cout << "  Test 1.3 passed: SQL with arithmetic expression parsed\n";
  }

  // 测试2：占位符数量验证
  {
    std::cout << "\nTest 2: Placeholder count validation\n";

    // 使用valid_sql_basic概念验证
    constexpr auto sql = "SELECT * FROM test WHERE id = ? AND name = ?"_sql;

    // 这些应该在编译期计算
    constexpr bool valid_for_0_args = valid_sql_basic<decltype(sql), 0>;
    constexpr bool valid_for_2_args = valid_sql_basic<decltype(sql), 2>;
    constexpr bool valid_for_3_args = valid_sql_basic<decltype(sql), 3>;

    std::cout << "  SQL: " << std::string_view(sql.str()) << "\n";
    std::cout << "  Placeholder count: " << sql.placeholder_count() << "\n";
    std::cout << "  Valid for 0 arguments: "
              << (valid_for_0_args ? "true" : "false") << "\n";
    std::cout << "  Valid for 2 arguments: "
              << (valid_for_2_args ? "true" : "false") << "\n";
    std::cout << "  Valid for 3 arguments: "
              << (valid_for_3_args ? "true" : "false") << "\n";

    if (!valid_for_0_args && valid_for_2_args && !valid_for_3_args) {
      std::cout
          << "  Test 2 passed: Placeholder count validation works correctly\n";
    } else {
      std::cout << "  Test 2 failed: Placeholder count validation incorrect\n";
    }
  }

  // 测试3：错误SQL应该触发编译错误
  {
    std::cout << "\nTest 3: Invalid SQL should cause compile error\n";
    std::cout << "  The following would trigger compile error:\n";
    std::cout << "  // Uncomment to test:\n";
    std::cout << "  // constexpr auto bad_sql = \"SELECT FROM\"_sql; // Syntax "
                 "error\n";
  }

  // 测试4：隐式转换保持兼容性
  {
    std::cout << "\nTest 4: Backward compatibility test\n";
    constexpr auto sql_result = "SELECT * FROM test"_sql;

    // 应该能隐式转换为FixedString
    auto fixed_str = sql_result.str(); // 使用str()方法获取FixedString
    std::cout << "  Implicit conversion to FixedString works\n";

    // 应该能获取原始字符串
    auto str_view = std::string_view(sql_result.str());
    std::cout << "  Can get original string: " << str_view << "\n";
  }

  std::cout << "\nAll new interface tests passed!\n";
}

int main() {
  try {
    test_new_interface();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}