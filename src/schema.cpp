#include <core.hpp>

using namespace ess::orm;

struct Inventory {
  int id = 0;
  int price = 0;
  static int cnt;
  using Schema = dsl::Schema<
      "inventory", //
      dsl::Field<"id", &Inventory::id, attribute::SerializedName<"price">,
                 attribute::AutoIncrement, attribute::PrimaryKey>, //
      dsl::Field<"price", &Inventory::price>,
      dsl::Field<"count", &Inventory::cnt> //
      >;
};

int Inventory::cnt = 2;

template <size_t N> void println(const ess::meta::FixedString<N> &str) {
  fmt::println("{}", std::string_view(str.m_str, N - 1));
}

int main() {
  Inventory i{.price = 1};
  static_assert(attribute::valid_attribute<int, attribute::AutoIncrement>);

  auto table_name = Inventory::Schema::table_name;
  auto fields = Inventory::Schema::make_fields();
  auto price_field = std::get<0>(fields);
  auto price_ptr = decltype(price_field)::pointer;
  auto ptr = &Inventory::price;
  static_assert(std::is_same_v<decltype(price_ptr), decltype(ptr)>);

  using price_attrs = decltype(price_field)::attributes;
  using price_first_attr = std::tuple_element_t<0, price_attrs>;
  auto price_name = price_first_attr::name;

  fmt::println("{}", i.*price_ptr);
  println(price_name);

  constexpr ess::meta::FixedString l = "asdfsadf";
  constexpr ess::meta::FixedString r = "asdfsadf";

  static_assert(ess::meta::fixed_string_is_equal<l, r>());

  // price_field
  // using attributes = std::tuple_element_t<

  // auto one = std::get<1>(fields);
  // println(one.column_name);

  // fmt::println("{}", *(one.pointer));

  // static_assert(std::is_same_v<tmp1::pointer_type, int *>);
  // int x = 10;
  // auto func = [&x]() { x++; };
}
