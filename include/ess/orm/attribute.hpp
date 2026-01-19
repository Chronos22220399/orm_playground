#pragma once
#include <ess/orm/common_concept.hpp>
#include <ess/orm/meta.hpp>

namespace ess::orm::attribute {

namespace detail {
// 属性标签，作为属性的接口

struct AttributeTag {};

} // namespace detail

// 默认表达式校验
template <meta::FixedString Expr>
constexpr bool is_valid_default_expr =
    (Expr.size() > 0 && Expr.template get<0>() != '\'' &&
     Expr.template get<0>() != '\"' &&
     Expr.template get<Expr.size() - 2>() != '\'' &&
     Expr.template get<Expr.size() - 2>() != '\"');

// 接口校验
template <typename Attr>
concept attribute_type =
    std::derived_from<Attr, attribute::detail::AttributeTag>;

// 主键
struct PrimaryKey : detail::AttributeTag {};

// 唯一值
struct Unique : detail::AttributeTag {};

// 自增
struct AutoIncrement : detail::AttributeTag {};

// 非空
struct NotNull : detail::AttributeTag {};

// 序列化后的键的名称
template <meta::FixedString Name> struct SerializedName : detail::AttributeTag {
  static constexpr meta::FixedString name = Name;
};

// sql 默认值
template <concepts::sql_default_value auto Value>
struct DefaultValue : detail::AttributeTag {
  using semantic_type = meta::sql_value_tag<Value>::type;
  static constexpr auto value = Value;
};

// 默认表达式
template <meta::FixedString Expr> struct DefaultExpr : detail::AttributeTag {
  using type = meta::sql_expr;
  static constexpr auto expr = Expr;

  static_assert(is_valid_default_expr<expr>, "\n默认表达式错误：\n"
                                             "1. 表达式长度必须大于1\n"
                                             "2. 表达式内部不包含引号\n");
};

namespace detail {
// 兜底
template <typename SqlSemantic, typename Attr>
struct valid_attribute_semantic : std::false_type {};

// 通用
template <typename SqlSemantic>
struct valid_attribute_semantic<SqlSemantic, PrimaryKey> : std::true_type {};

template <typename SqlSemantic>
struct valid_attribute_semantic<SqlSemantic, Unique> : std::true_type {};

template <typename SqlSemantic>
struct valid_attribute_semantic<SqlSemantic, NotNull> : std::true_type {};

template <typename SqlSemantic, meta::FixedString Name>
struct valid_attribute_semantic<SqlSemantic, SerializedName<Name>>
    : std::true_type {};

// integer
template <>
struct valid_attribute_semantic<meta::sql_integer, AutoIncrement>
    : std::true_type {};

// default value
template <typename ColumnSemantic, auto Value>
struct valid_attribute_semantic<ColumnSemantic, DefaultValue<Value>>
    : std::bool_constant<meta::sql_compatible_v<
          ColumnSemantic, typename meta::sql_value_tag<Value>::type>> {};

// default expression
template <typename SqlSemantic, meta::FixedString Expr>
struct valid_attribute_semantic<SqlSemantic, DefaultExpr<Expr>>
    : std::true_type {};

//
template <typename MemberType, typename Attr> struct valid_attribute_impl {
private:
  using column_semantic = meta::cpp_type_to_sql_semantic_t<MemberType>;

public:
  static constexpr bool value =
      valid_attribute_semantic<column_semantic, Attr>::value;
};

template <typename Attr> struct attribute_category {
  using type = Attr;
};

template <meta::FixedString Name>
struct attribute_category<SerializedName<Name>> {
  struct serialized_name_tag {};
  using type = serialized_name_tag;
};

struct default_value_or_expr_tag {};
template <auto Value> struct attribute_category<DefaultValue<Value>> {
  using type = default_value_or_expr_tag;
};

template <meta::FixedString Expr> struct attribute_category<DefaultExpr<Expr>> {
  using type = default_value_or_expr_tag;
};

// 检查是否有同类属性
// 检查属性 I 是否在前面出现过
template <typename Tuple, size_t I> constexpr bool check_at_prev() {
  // 第0个表示未出现
  if constexpr (I == 0) {
    return false;
  }

  // 将第 I 个属性与前 [0, I) 个属性分别对比，查看是否重复过
  using current_ctg =
      typename attribute_category<std::tuple_element_t<I, Tuple>>::type;
  bool found = false;

  [&]<size_t... Prev>(std::index_sequence<Prev...>) {
    return (
        (found =
             (found ||
              std::is_same_v<current_ctg,
                             typename attribute_category<
                                 std::tuple_element_t<Prev, Tuple>>::type>)),
        ...);
  }(std::make_index_sequence<I>{});
  return found;
}

// 检查是否有同类属性
template <typename Tuple> constexpr bool has_dup_attrs_in_tuple() {
  constexpr size_t N = std::tuple_size_v<Tuple>;
  if (N <= 1) {
    return false;
  }

  bool dup = false;
  constexpr auto i_seq = std::make_index_sequence<N>{};
  [&]<size_t... I>(std::index_sequence<I...>) {
    return ((dup = (dup || check_at_prev<Tuple, I>())), ...);
  }(i_seq);
  return dup;
}
} // namespace detail

template <typename MemberType, typename Attr>
concept valid_attribute = detail::valid_attribute_impl<MemberType, Attr>::value;

// 检查一个属性是否合法（与绑定的成员类型匹配、值合法）
template <typename MemberType, typename Attr>
constexpr void check_one_attribute() {
  static_assert(attribute::attribute_type<Attr>, "不能使用的属性");

  static_assert(attribute::valid_attribute<MemberType, Attr>,
                "\n属性验证失败，请检查是否使用了不匹配的属性，特别检查默认值类"
                "型是否和绑定的成员类型相同\n");
}

// 检查所有属性
template <typename MemberType, typename... Attrs>
constexpr void check_attributes() {
  (check_one_attribute<MemberType, Attrs>(), ...);
}

// 检查是否有同类属性
template <typename Tuple>
constexpr bool has_dup_attrs_in_tuple = detail::has_dup_attrs_in_tuple<Tuple>();

// 萃取出生成 sql ddl 所需字符串
struct PlaceHolder {};

template <typename, auto> struct attr_traits {};

template <> struct attr_traits<PrimaryKey, PlaceHolder{}> {
  static constexpr auto attr_str = meta::FixedString{"PRIMARY KEY"};
};

template <> struct attr_traits<Unique, PlaceHolder{}> {
  static constexpr auto attr_str = meta::FixedString{"UNIQUE"};
};

template <> struct attr_traits<NotNull, PlaceHolder{}> {
  static constexpr auto attr_str = meta::FixedString{"NOT NULL"};
};

template <> struct attr_traits<AutoIncrement, PlaceHolder{}> {
  static constexpr auto attr_str = meta::FixedString{"AUTOINCREMENT"};
};

template <auto Value> struct attr_traits<DefaultValue<Value>, Value> {
  static constexpr auto attr_str = meta::FixedString("DEFAULT ");
};

template <meta::FixedString Expr> struct attr_traits<DefaultExpr<Expr>, Expr> {
  static constexpr auto attr_str = meta::FixedString("DEFAULT ");
};

// 从属性映射为字符串
template <typename Attr> std::string to_sql_fragment(Attr) {
  using T = std::remove_cvref_t<Attr>;

  if constexpr (requires { attr_traits<T, PlaceHolder{}>::attr_str; }) {
    return std::string(
        std::string_view(attr_traits<T, PlaceHolder{}>::attr_str));
  } else if constexpr (requires { T::value; }) {
    using Trait = attr_traits<T, T::value>;
    using ValueType = std::remove_cvref_t<decltype(T::value)>;

    // 默认将 enum 转换为 int
    if constexpr (std::is_enum_v<ValueType>) {
      return fmt::format("{} {}", Trait::attr_str, static_cast<int>(T::value));
    } else {
      return fmt::format("{} {}", Trait::attr_str, T::value);
    }
  } else if constexpr (requires { T::expr; }) {
    using Trait = attr_traits<T, T::expr>;
    return fmt::format("{} {}", Trait::attr_str, T::expr);
  }
  return "";
}

} // namespace ess::orm::attribute
