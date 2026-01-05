#pragma once
#include <core.hpp>

namespace ess::orm::concepts {
template <auto Ptr>
concept not_null_pointer = (Ptr != nullptr);

// 检查是否为 FixedString  类型
template <typename> struct is_fixed_string_impl : std::false_type {};

template <std::size_t N>
struct is_fixed_string_impl<ess::meta::FixedString<N>> : std::true_type {};

// 检查是否为 SqlStringLiteral  类型
template <typename T>
concept is_fixed_string = is_fixed_string_impl<T>::value;

template <typename> struct is_sql_string_literal_impl : std::false_type {};

template <typename>
struct is_sql_value_impl : std::false_type {};

template <std::integral T>
  struct is_sql_value_impl<T>: std::true_type {};

template <std::floating_point T>
struct is_sql_value_impl<T> : std::true_type {};

template <> struct is_sql_value_impl<bool> : std::true_type {};

template <typename E>
  requires std::is_enum_v<E>
struct is_sql_value_impl<E> : std::true_type {};

template <is_fixed_string S>
struct is_sql_value_impl<S> : std::true_type {};

template <> struct is_sql_value_impl<meta::SqlNull> : std::true_type {};

template <> struct is_sql_value_impl<meta::SqlNow> {};

template <typename T>
concept sql_default_value = is_sql_value_impl<T>::value;
} // namespace ess::orm::concepts
