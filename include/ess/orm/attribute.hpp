#pragma once
#include <ess/meta.hpp>
#include <ess/orm/common_concept.hpp>

namespace ess::orm::attribute {

namespace detail {
// 属性标签，作为属性的接口
struct AttributeTag {
  // template <typename T> static consteval bool accept() { return true; }
};

// 默认表达式校验
template <ess::meta::FixedString Expr>
constexpr bool is_valid_default_expr =
    (Expr.size() > 0 && Expr.template get<0>() != '\'' &&
     Expr.template get<0>() != '\"' &&
     Expr.template get<Expr.size() - 2>() != '\'' &&
     Expr.template get<Expr.size() - 2>() != '\"');
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
  // template <std::integral> static consteval bool accept() { return true; }
  // template <typename> static consteval bool accept() { return false; }
};

// 非空
struct NotNull : detail::AttributeTag {};

// 序列化后的键的名称
template <meta::FixedString Name> struct SerializedName : detail::AttributeTag {
  static constexpr meta::FixedString name = Name;
};

// 默认值
template <auto> struct DefaultValue {};

template <concepts::sql_default_value auto Value>
struct DefaultValue<Value> : detail::AttributeTag {
  static constexpr auto value = Value;
};

// 默认表达式
template <ess::meta::FixedString Expr>
struct DefaultExpr : detail::AttributeTag {
  static constexpr auto expr = Expr;

  static_assert(detail::is_valid_default_expr<expr>,
                "\n默认表达式错误：\n"
                "1. 表达式长度必须大于1\n"
                "2. 表达式内部不包含引号\n");
};

template <typename T, typename Attr>
struct is_valid_attribute : std::false_type {};

template <std::integral T>
struct is_valid_attribute<T, AutoIncrement> : std::true_type {};

template <typename T>
struct is_valid_attribute<T, PrimaryKey> : std::true_type {};

template <typename T, ess::meta::FixedString Str>
struct is_valid_attribute<T, SerializedName<Str>> : std::true_type {};

// template <typename T>
// struct is_valid_attribute<T, DefaultValue<V>> : std::is_sql_int_literal<V>
// {};

// template <typename T, auto V>
// struct is_valid_attribute<T, DefaultValue<V>>
//     : std::bool_constant<concepts::sql_default_value<decltype(V)>> {};

template <typename MemberType, typename Attr>
concept valid_attribute = is_valid_attribute<MemberType, Attr>::value;

// template <typename MemberType, typename Attr>
// concept valid_attribute =
//     is_attribute_type<MemberType> && Attr::template accept<MemberType>();
} // namespace ess::orm::attribute
