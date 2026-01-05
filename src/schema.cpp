#include <core.hpp>

using namespace ess::orm;

struct Inventory {
  int id = 0;
  int price = 0;
  std::string name = "";

  // TODO:
  // 1. 当前未做对重复同一类属性的检测，例如可能存在两个 SerializedName
  // 2. DefaultValue 还不支持字符串
  using Schema = dsl::Schema<
      "inventory", //
      dsl::Field<"id", &Inventory::id, attribute::SerializedName<"id">,
                 attribute::AutoIncrement, attribute::PrimaryKey>, //
      dsl::Field<"price", &Inventory::price>,                      //
      dsl::Field<"name", &Inventory::name,
                 attribute::SerializedName<"inventory_name">>>;
};

template <size_t N> void println(const ess::meta::FixedString<N> &str) {
  fmt::println("{}", std::string_view(str.m_str, N - 1));
}

using namespace ess::meta;

int main() {
  Inventory i{.id = 10, .price = 1};
  static_assert(attribute::valid_attribute<int, attribute::AutoIncrement>);

  using fields = Inventory::Schema::fields;
  using first_field = std::tuple_element_t<0, fields>;
  auto field_name = first_field::column_name;
  auto pointer = first_field::pointer;
  using pointer_type = first_field::pointer_type;

  static_assert(std::is_same_v<pointer_type, int Inventory::*>);

  fmt::println("{}", i.*pointer);

  using expr = attribute::DefaultExpr<"helo"_fs>;
  auto tmp = expr::expr;

  auto value_1 = attribute::DefaultValue<10>::value;
  auto value_2 = attribute::DefaultValue<1.2>::value;
  auto value_3 = attribute::DefaultValue<"hello"_fs>::value;

  println(tmp);
  fmt::println("{}", value_1);
  println(value_3);
}
