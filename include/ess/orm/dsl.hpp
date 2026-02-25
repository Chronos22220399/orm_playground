#pragma once
#include <ess/orm/attribute.hpp>
#include <ess/orm/common_concept.hpp>
#include <ess/orm/meta.hpp>
#include <ess/orm/traits.hpp>
#include <fmt/ranges.h>

namespace ess::orm::dsl {
template <meta::FixedString ColumnName, // column name
          auto Ptr = nullptr, // member pointer (can be nullptr for future
                              // expendation)
          typename... Attrs   // attributes for sql defination
          >
struct Field {
private:
  using member_traits = ess::orm::traits::MemberPointerTraits<Ptr>;

public:
  using pointer_type = typename member_traits::pointer_type;
  using member_type = member_traits::member_type;

  static constexpr meta::FixedString column_name = ColumnName;
  static constexpr decltype(auto) pointer = member_traits::pointer;
  using attributes = std::tuple<Attrs...>;

private:
  static constexpr bool _check() {
    // 检查属性是否合法
    attribute::check_attributes<member_type, Attrs...>();
    return true;
  }
  static_assert(_check());
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

// TODO: 后续优化成编译期的函数从而减少模版实例化
template <typename...> struct no_duplicate_detector;

template <> struct no_duplicate_detector<> : std::true_type {};

template <typename Field>
struct no_duplicate_detector<Field> : std::true_type {};

template <typename First, typename... Rest>
struct no_duplicate_detector<First, Rest...>
    : std::bool_constant<((!meta::fs_equal(First::column_name,
                                           Rest::column_name) &&
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

private:
  // TODO: 将 make_create_table_ddl 拆分出来，让 Schema 仅作为元数据载体
  template <typename Field>
  [[gnu::noinline]] static std::string make_col_def() {
    using member_semantic_type =
        meta::cpp_type_to_sql_semantic_t<typename Field::member_type>;
    auto attributes = typename Field::attributes{};
    // 字段名 类型
    std::string col_def{std::string_view(Field::column_name)};
    col_def += " ";
    col_def += std::string_view(
        meta::sql_semantic_to_type_str<member_semantic_type>::type_str);

    // 属性
    std::string attrs_str{};
    std::apply(
        [&](auto... attrs) {
          ((attrs_str += " " + attribute::to_sql_fragment(attrs)), ...);
        },
        attributes);

    return col_def + attrs_str;
  }

public:
  static auto make_fields() { return std::make_tuple(Fields{}...); }

  static std::string make_create_table_ddl(bool not_replace = true) {
    std::vector<std::string> col_defs{};
    // 预留字段
    col_defs.reserve(sizeof...(Fields));
    (col_defs.push_back(make_col_def<Fields>()), ...);
    return fmt::format(
        "CREATE TABLE {}{} ({});", (not_replace ? "IF NOT EXISTS " : ""),
        std::string_view(table_name), meta::join(col_defs, ",\n"));
  }

private:
  static_assert(no_duplicated_key_field_words<Fields...>,
                "存在多个不同Field的名称或是绑定的成员指针相同");
  // TODO:
  // 添加 Not Null
};

} // namespace ess::orm::dsl
