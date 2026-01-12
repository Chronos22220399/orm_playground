#pragma once
#include <ess/orm/meta.hpp>

namespace ess::orm::concepts {
namespace detail {
// 检查是否为 FixedString  类型
template <typename> struct is_fixed_string_impl : std::false_type {};

template <std::size_t N>
struct is_fixed_string_impl<meta::FixedString<N>> : std::true_type {};

// 兜底
template <typename> struct is_sql_value_impl : std::false_type {};

// 整形 sql 值
template <std::integral T> struct is_sql_value_impl<T> : std::true_type {};

// 浮点形 sql 值
template <std::floating_point T>
struct is_sql_value_impl<T> : std::true_type {};

// 布尔形 sql 值
template <> struct is_sql_value_impl<bool> : std::true_type {};

// 枚举型 sql 值
template <typename E>
  requires std::is_enum_v<E>
struct is_sql_value_impl<E> : std::true_type {};
} // namespace detail

template <auto Ptr>
concept not_null_pointer = (Ptr != nullptr);

// 检查是否为 SqlStringLiteral  类型
template <typename T>
concept fixed_string_type =
    detail::is_fixed_string_impl<std::remove_cvref_t<T>>::value;

namespace detail {
// 字符串型
template <fixed_string_type S> struct is_sql_value_impl<S> : std::true_type {};
// 空值
template <> struct is_sql_value_impl<meta::SqlNull> : std::true_type {};
// 当前时间点
template <> struct is_sql_value_impl<meta::SqlNow> : std::true_type {};

} // namespace detail

// 支持 integer、floating_point、enum、bool、fixed_string
template <typename T>
concept sql_default_value = detail::is_sql_value_impl<T>::value;
} // namespace ess::orm::concepts
