#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <ess/orm/core_orm.hpp>

using namespace ess::orm;
using namespace ess::meta;
TEST_CASE("DefaultExpr check") {
  CHECK_FALSE(attribute::detail::is_valid_default_expr<"'helo'"_fs>);
  CHECK_FALSE(attribute::detail::is_valid_default_expr<"helo'"_fs>);
  CHECK_FALSE(attribute::detail::is_valid_default_expr<"'helo"_fs>);
  CHECK_FALSE(attribute::detail::is_valid_default_expr<"helo'"_fs>);
}

TEST_CASE("DefaultValue check") {}
