#include <ess/orm/orm.hpp>
#include <iostream>

using namespace ess::orm;
using namespace ess::orm::sql;

// 测试占位符数量不匹配（应该触发编译错误）
int main() {
  // 这个测试应该触发编译错误
  // 因为SQL有1个占位符，但query期望0个参数

  constexpr auto sql = "SELECT * FROM test WHERE id = ?"_sql;

  // 尝试调用query，应该因为requires约束失败而无法编译
  // query<sql>(); // 应该失败：1个占位符 vs 0个参数

  std::cout
      << "If you see this, the test didn't trigger compile error as expected\n";
  return 0;
}