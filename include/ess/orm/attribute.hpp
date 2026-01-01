#pragma once
#include <ess/meta.hpp>
#include <ess/orm/common_concept.hpp>

namespace ess::orm::attribute {

namespace detail {
// 属性标签，作为属性的接口
struct AttributeTag {
  template <typename T> static consteval bool accept() { return true; }
};
} // namespace detail

// 接口校验
template <typename Attr>
concept is_attribute_type =
    std::derived_from<Attr, attribute::detail::AttributeTag>;

// 主键
struct PrimaryKey : detail::AttributeTag {};

// 唯一值
struct Unique : detail::AttributeTag {};

// 自增
struct AutoIncrement : detail::AttributeTag {
  template <std::integral> static consteval bool accept() { return true; }
  template <typename> static consteval bool accept() { return false; }
};

// 非空
struct NotNull : detail::AttributeTag {};

// 序列化后的键的名称
template <meta::FixedString Name> struct SerializedName : detail::AttributeTag {
  static constexpr meta::FixedString name = Name;
};

// 默认值
template <auto Value>
  requires concepts::is_sql_literal_type<decltype(Value)>
struct DefaultValue : detail::AttributeTag {
  static constexpr auto value = Value;

  template <std::integral> static consteval bool accept() { return true; }

  // static_assert(accept<Value>(), "不是合规的sql字面量");
};

template <meta::FixedString Expr> struct DefaultExpr : detail::AttributeTag {
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

template <typename T, T Value>
struct is_valid_attribute<T, DefaultValue<Value>> : std::true_type {};

template <typename MemberType, typename Attr>
concept valid_attribute = is_valid_attribute<MemberType, Attr>::value;

// template <typename MemberType, typename Attr>
// concept valid_attribute =
//     is_attribute_type<MemberType> && Attr::template accept<MemberType>();
} // namespace ess::orm::attribute
