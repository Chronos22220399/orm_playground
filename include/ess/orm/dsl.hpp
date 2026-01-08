#pragma once
#include <ess/orm/core_orm.hpp>
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
  static constexpr meta::FixedString column_name = ColumnName;
  using pointer_type = typename member_traits::pointer_type;
  static constexpr decltype(auto) pointer = member_traits::pointer;
  using member_type = member_traits::member_type;
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

  static auto make_fields() { return std::make_tuple(Fields{}...); }

  // 优化2
  // 1. 将单列处理逻辑提取为一个独立的、非内联的辅助函数模板
  // template <typename Field>
  // [[gnu::noinline]] static std::string make_column_def() {
  //   using member_type = typename Field::member_type;
  //   using sql_type = meta::cpp_type_to_sql_semantic_t<member_type>;
  //
  //   std::string def = std::string(std::string_view(Field::column_name));
  //   def += " ";
  //   def += std::string(
  //       std::string_view(meta::sql_semantic_to_type_str<sql_type>::type_str));
  //
  //   // 处理属性
  //   std::apply(
  //       [&](auto... attrs) {
  //         ((def += " ", def += attribute::to_sql_fragment(attrs)), ...);
  //       },
  //       typename Field::attributes{});
  //
  //   return def;
  // }
  //
  // // 2. 在 Schema 中调用
  // static std::string make_create_table_ddl() {
  //   std::vector<std::string> column_defs;
  //   column_defs.reserve(sizeof...(Fields));
  //
  //   // 使用简单的折叠表达式调用独立函数
  //   (column_defs.push_back(make_column_def<Fields>()), ...);
  //
  //   return fmt::format("CREATE TABLE {} ({});", std::string_view(table_name),
  //                      fmt::join(column_defs, ",\n"));
  // }

  // 优化1
  // static std::string make_create_table_ddl() {
  //   auto process_field = []<typename F>() {
  //     // 直接返回该列的片段，减少函数体大小
  //     using sql_type =
  //         meta::cpp_type_to_sql_semantic_t<typename F::member_type>;
  //     std::string col = fmt::format(
  //         "{} {}", std::string_view(F::column_name),
  //         std::string_view(meta::sql_semantic_to_type_str<sql_type>::type_str));
  //
  //     // 处理属性
  //     std::string attrs_str;
  //     std::apply(
  //         [&](auto... attrs) {
  //           ((attrs_str += " ", attrs_str +=
  //           attribute::to_sql_fragment(attrs)),
  //            ...);
  //         },
  //         typename F::attributes{});
  //
  //     return col + attrs_str;
  //   };
  //
  //   // 关键优化点：直接用折叠表达式收集结果
  //   std::vector<std::string> column_defs;
  //   column_defs.reserve(sizeof...(Fields)); // 提前扩容
  //   (column_defs.push_back(process_field.template operator()<Fields>()),
  //   ...);
  //
  //   return fmt::format("CREATE TABLE {} ({});", std::string_view(table_name),
  //                      fmt::join(column_defs, ",\n"));
  // }

  // 原本
  static constexpr std::string make_create_table_ddl() {
    std::vector<std::string> column_defs{};

    (
        [&]() {
          std::vector<std::string> parts{};
          // 列名
          parts.push_back(std::string(std::string_view(Fields::column_name)));
          // 类型名
          using sql_type =
              meta::cpp_type_to_sql_semantic_t<typename Fields::member_type>;
          parts.push_back(std::string(std::string_view(
              meta::sql_semantic_to_type_str<sql_type>::type_str)));
          // 属性列表
          std::apply(
              [&](auto... attrs) {
                (parts.push_back(attribute::to_sql_fragment(attrs)), ...);
              },
              typename Fields::attributes{});

          // 过滤掉空字符串并拼接
          std::string def;
          for (const auto &p : parts) {
            if (!p.empty())
              def += " ";
            def += p;
          }
          column_defs.push_back(def);
        }(),
        ...);
    return fmt::format("CREATE TABLE {} ({});", std::string_view(table_name),
                       fmt::join(column_defs, ",\n"));
  }

private:
  static_assert(no_duplicated_key_field_words<Fields...>,
                "存在多个不同Field的名称或是绑定的成员指针相同");
};

} // namespace ess::orm::dsl
