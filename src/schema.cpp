#include <core.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/runtime.hpp>
// #include <ess/orm/test/stress_test.hpp>

using namespace ess::orm;
using namespace ess::orm::meta;

enum class GoodsStatus { Normal = 0, Disabled, Deleted };

struct Goods {
  long long id = 0;
  std::string title;
  double price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;

  using Schema = dsl::Schema<
      "goods",
      dsl::Field<"id", &Goods::id, attribute::PrimaryKey,
                 attribute::AutoIncrement, attribute::DefaultValue<1>>,
      dsl::Field<"title", &Goods::title,
                 attribute::DefaultValue<"untitled"_fs>>,
      dsl::Field<"price", &Goods::price, attribute::DefaultValue<0.0>>,
      dsl::Field<"stock", &Goods::stock, attribute::DefaultValue<0>>,
      dsl::Field<"status", &Goods::status,
                 attribute::DefaultValue<GoodsStatus::Deleted>>,
      dsl::Field<"enabled", &Goods::enabled, attribute::DefaultValue<true>>>;
};

template <size_t N> void println(const ess::orm::meta::FixedString<N> &str) {
  fmt::println("{}", std::string_view(str));
}

struct Foo {};

namespace ess::orm::dialect {
constexpr Postgres get_orm_dialect(DialectTag) { return {}; }
} // namespace ess::orm::dialect

int main() {
  auto goods_ddl = Goods::Schema::make_create_table_ddl();
  // fmt::print(fmt::fg(fmt::color::aquamarine), "--- Goods DDL ---\n{}\n\n",
  //            goods_ddl);

  ess::orm::query<Goods, "SELECT * FROM goods WHERE id > ?">(10);

  return 0;
}
