#include <core.hpp>

using namespace ess::orm;

struct Inventory {
  int id = 0;
  int price = 0;
  std::string name = "";

  using Schema = dsl::Schema<
      "inventory", //
      dsl::Field<"id", &Inventory::id, attribute::SerializedName<"price">,
                 attribute::AutoIncrement, attribute::PrimaryKey,
                 ess::orm::attribute::DefaultValue<10>>, //
      dsl::Field<"price", &Inventory::price>,            //
      dsl::Field<"name", &Inventory::name,
                 attribute::SerializedName<"inventory_name">>>;
};

template <size_t N> void println(const ess::meta::FixedString<N> &str) {
  fmt::println("{}", std::string_view(str.m_str, N - 1));
}

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

  // auto table_name = Inventory::Schema::table_name;
  // auto fields = Inventory::Schema::make_fields();
  // auto price_field = std::get<0>(fields);
  // auto price_ptr = decltype(price_field)::pointer;
  // auto ptr = &Inventory::price;
  // static_assert(std::is_same_v<decltype(price_ptr), decltype(ptr)>);
  //
  // using price_attrs = decltype(price_field)::attributes;
  // using price_first_attr = std::tuple_element_t<0, price_attrs>;
  // auto price_name = price_first_attr::name;
  //
  // fmt::println("{}", i.*price_ptr);
  // println(price_name);
}
