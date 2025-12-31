#pragma once
#include <ess/meta.hpp>

namespace ess::orm::attribute {

namespace detail {
struct AttributeTag {};
} // namespace detail

template <typename Attr>
concept is_attribute_type =
    std::derived_from<Attr, attribute::detail::AttributeTag>;

struct PrimaryKey : detail::AttributeTag {};

struct Unique : detail::AttributeTag {};

struct AutoIncrement : detail::AttributeTag {};

struct NotNull : detail::AttributeTag {};

template <meta::FixedString Name> struct SerializedName : detail::AttributeTag {
  static constexpr meta::FixedString name = Name;
};

template <auto Value>
// requires is_
struct DefaultValue {
  static constexpr auto value = Value;
};

template <meta::FixedString Expr> struct DefaultExpr {
  static_assert(                                    //
      Expr.size() > 1 &&                            //
      Expr.template get<0>() != '\'' &&             //
      Expr.template get<0>() != '\"' &&             //
      Expr.template get<Expr.size() - 2> != '\'' && //
      Expr.template get<Expr.size() - 2> != '\"'    //
  );
  static constexpr meta::FixedString expr = Expr;
};

template <typename T, typename Attr>
struct is_valid_attribute : std::false_type {};

template <std::integral T>
struct is_valid_attribute<T, AutoIncrement> : std::true_type {};

template <typename T>
struct is_valid_attribute<T, PrimaryKey> : std::true_type {};

template <typename T, meta::FixedString Name>
struct is_valid_attribute<T, SerializedName<Name>> : std::true_type {};

template <typename MemberType, typename Attr>
concept valid_attribute = is_valid_attribute<MemberType, Attr>::value;

} // namespace ess::orm::attribute
