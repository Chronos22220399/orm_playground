#pragma once
#include <ess/orm/traits.hpp>

namespace ess::orm::dsl {
template <meta::FixedString ColumnName, // column name
          auto Ptr = nullptr, // member pointer (can be nullptr for future
                              // expendation)
          typename... Attrs   // attributes for sql defination
          >
// requires(
//     (attribute::is_attribute_type<Attrs> && ...) &&
//     (attribute::valid_attribute<
//          typename traits::MemberPointerTraits<Ptr>::member_type, Attrs> &&
//      ...))
struct Field {
private:
  using member_traits = ess::orm::traits::MemberPointerTraits<Ptr>;

public:
  static constexpr meta::FixedString column_name = ColumnName;
  using pointer_type = typename member_traits::pointer_type;
  static constexpr decltype(auto) pointer = member_traits::pointer;
  using attributes = std::tuple<Attrs...>;

private:
  static_assert((attribute::is_attribute_type<Attrs> && ...),
                "有一个或多个不能使用的属性");
  static_assert(
      (attribute::valid_attribute<
           typename traits::MemberPointerTraits<Ptr>::member_type, Attrs> &&
       ...),
      "发现了有一个或多个不合规属性");
};

template <typename T> struct is_field : std::false_type {};

template <meta::FixedString FiledName, auto Ptr, typename... Attrs>
struct is_field<Field<FiledName, Ptr, Attrs...>> : std::true_type {};

template <typename T>
concept field_type = is_field<T>::value;

template <typename LField, typename RField> consteval bool is_same_binding() {
  using L_traits = traits::MemberPointerTraits<LField::pointer>;
  using R_traits = traits::MemberPointerTraits<RField::pointer>;

  if constexpr (!std::is_same_v<typename L_traits::member_type,
                                typename R_traits::member_type>) {
    return false;
  } else if constexpr (L_traits::is_static != R_traits::is_static) {
    return false;
  } else {
    return L_traits::pointer == R_traits::pointer;
  }
}

template <typename...> struct no_duplicate_detector;

template <> struct no_duplicate_detector<> : std::true_type {};

template <typename Field>
struct no_duplicate_detector<Field> : std::true_type {};

template <typename First, typename... Rest>
struct no_duplicate_detector<First, Rest...>
    : std::bool_constant<((!meta::fixed_string_is_equal<First::column_name,
                                                        Rest::column_name>() &&
                           (!is_same_binding<First, Rest>())) &&
                          ...) &&
                         no_duplicate_detector<Rest...>::value> {};

template <typename... Fields>
concept no_duplicated_key_field_words = no_duplicate_detector<Fields...>::value;

// TODO: 添加全局作用域的Field
template <typename... MemPtrs> struct GlobalPrimaryKey {
  using member_pointers = std::tuple<MemPtrs...>;
};

// Schema
template <meta::FixedString TableName, field_type... Fields>
  requires(no_duplicated_key_field_words<Fields...>)
struct Schema {
  static constexpr meta::FixedString table_name = TableName;
  using fields = std::tuple<Fields...>;

  static auto make_fields() { return std::make_tuple(Fields{}...); }

private:
  static_assert(no_duplicated_key_field_words<Fields...>,
                "存在多个不同Field的关键字相同");
};

} // namespace ess::orm::dsl
