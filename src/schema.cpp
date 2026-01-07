#include <core.hpp>

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
  fmt::println("{}", std::string_view(str.m_str, N - 1));
}

int main() {
  constexpr auto str = "hello"_fs;
  auto s = fs_substr_view(str, 1, 3);
  fmt::println("{}", s);
  fmt::println("{}", fs_string_view(str));
}
