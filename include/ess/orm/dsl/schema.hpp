#pragma once
#include <ess/orm/common/concept.hpp>
#include <ess/orm/common/meta.hpp>
#include <ess/orm/dsl/attribute.hpp>
#include <ess/orm/dsl/field.hpp>

namespace ess::orm::dsl {

// TODO: 后续优化成编译期的函数从而减少模版实例化
template <typename...> struct no_duplicate_detector;

template <> struct no_duplicate_detector<> : std::true_type {};

template <typename Field>
struct no_duplicate_detector<Field>
    : std::bool_constant<(Field::column_name.size(), true)> {};

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
  requires(no_duplicated_key_field_words<Fields...>) // 用 requires 是 Fields
                                                     // 强制实例化
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
    std::string result = "CREATE TABLE ";
    result += (not_replace ? "IF NOT EXISTS " : "");
    result += std::string_view(table_name);
    result += " (";
    result += meta::join(col_defs, ",\n");
    result += ");";
    return result;
  }

private:
  static_assert(no_duplicated_key_field_words<Fields...>,
                "存在多个不同Field的名称或是绑定的成员指针相同");
  // TODO:
  // 添加 Not Null
};
} // namespace ess::orm::dsl
