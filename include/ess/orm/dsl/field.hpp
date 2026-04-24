#pragma once
#include <ess/orm/dsl/traits.hpp>

namespace ess::orm::dsl {
template <meta::FixedString ColumnName, // column name
          auto Ptr = nullptr, // member pointer (can be nullptr for future
                              // expendation)
          typename... Attrs   // attributes for sql defination
          >
struct Field {
private:
  using member_traits = traits::MemberPointerTraits<Ptr>;

public:
  using pointer_type = typename member_traits::pointer_type;
  using member_type = member_traits::member_type;
  using class_type = member_traits::class_type;

  static constexpr meta::FixedString column_name = ColumnName;
  static constexpr decltype(auto) pointer = member_traits::pointer;
  using attributes = std::tuple<Attrs...>;

private:
  // 检查属性是否合法
  static_assert((attribute::check_attributes<member_type, Attrs...>(), true),
                "attribute validation failed");
  // 检查是否存在重复同类别属性
  static_assert(!attribute::has_dup_attrs_in_tuple<attributes>,
                "\n存在重复类型的属性： \n"
                "1. 任意属性在一个Field中不能重复声明\n"
                "2. DefaultValue 与 DefaultExpr 互斥\n"
                "3. 不可存在多个 DefaultValue 或 DefaultExpr");
};

template <typename T> struct is_field : std::false_type {};

template <meta::FixedString FiledName, auto Ptr, typename... Attrs>
struct is_field<Field<FiledName, Ptr, Attrs...>> : std::true_type {};

template <typename T>
concept field_type = is_field<T>::value;

template <typename LField, typename RField> constexpr bool is_same_binding() {
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
} // namespace ess::orm::dsl
