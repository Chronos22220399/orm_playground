#include <ess/orm/sql/validator.hpp>
#include <iostream>

using namespace ess::orm::sql;

void test_error_locations() {
  std::cout << "Testing error location reporting...\n\n";

  std::cout << "=== CASE 1: Syntax error (missing table name) ===\n";
  std::cout << "SQL: SELECT * FROM WHERE id = ?\n";
  std::cout << "Error location: At _sql call site (compile time)\n";
  std::cout << "Error message: SQL Parser Error: Invalid SQL structure\n";
  std::cout << "Uncomment to test:\n";
  std::cout
      << "// constexpr auto bad_sql = \"SELECT * FROM WHERE id = ?\"_sql;\n\n";

  std::cout << "=== CASE 2: Semantic error (placeholder mismatch) ===\n";
  std::cout << "SQL: SELECT * FROM goods WHERE id = ?\n";
  std::cout << "Correct usage: query<Goods, \"SELECT * FROM goods WHERE id = "
               "?\"_sql>(1);\n";
  std::cout << "Error case 1 (missing arg): query<Goods, \"SELECT * FROM goods "
               "WHERE id = ?\"_sql>();\n";
  std::cout << "Error location: At query call site (compile time)\n";
  std::cout << "Error message: Constraints not satisfied (placeholder count "
               "mismatch)\n\n";

  std::cout << "Error case 2 (extra arg): query<Goods, \"SELECT * FROM goods "
               "WHERE id = ?\"_sql>(1, 2);\n";
  std::cout << "Error location: At query call site (compile time)\n";
  std::cout << "Error message: Constraints not satisfied (placeholder count "
               "mismatch)\n\n";

  std::cout << "=== CASE 3: Correct usage ===\n";
  std::cout << "SQL: SELECT * FROM goods WHERE id = ?\n";
  std::cout
      << "Usage: query<Goods, \"SELECT * FROM goods WHERE id = ?\"_sql>(1);\n";
  std::cout << "Result: Compiles successfully\n\n";

  std::cout << "=== CASE 4: Using FixedString (no validation) ===\n";
  std::cout << "SQL: SELECT * FROM WHERE id = ? (syntax error!)\n";
  std::cout << "Usage: query<Goods, \"SELECT * FROM WHERE id = ?\">(1);\n";
  std::cout << "Result: Compiles (no validation), runtime error\n";
  std::cout << "This allows bypassing validation when needed\n\n";

  std::cout << "=== Summary ===\n";
  std::cout << "1. Syntax errors → _sql call site\n";
  std::cout << "2. Semantic errors → query call site\n";
  std::cout << "3. FixedString bypasses all validation\n";
  std::cout << "4. SqlParseResult enables compile-time validation\n";
}

int main() {
  test_error_locations();
  return 0;
}