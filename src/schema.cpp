#include <core.hpp>
#include <ess/orm/test/stress_test.hpp>

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
                 attribute::AutoIncrement>,
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

int main() {
  Massive240 entity{};
  // 1. 生成所有 DDL
  auto goods_ddl = Goods::Schema::make_create_table_ddl();
  auto massive_ddl = Massive240::Schema::make_create_table_ddl();

  // 2. 彩色输出
  fmt::print(fmt::fg(fmt::color::aquamarine), "--- Goods DDL ---\n{}\n\n",
             goods_ddl);

  // Massive Entity 重点展示：使用金色显示
  // fmt::print(fmt::fg(fmt::color::gold) | fmt::emphasis::bold,
  //            "--- MASSIVE ENTITY (240 FIELDS STRESS TEST) ---\n");
  // fmt::print(fmt::fg(fmt::color::wheat), "{}\n\n", massive_ddl);

  return 0;
}
