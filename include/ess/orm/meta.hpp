#pragma once
#include <optional>

namespace ess::orm::meta {
template <std::size_t N> struct FixedString {
  char m_str[N];

  constexpr FixedString(const char (&str)[N]) {
    for (int i = 0; i < N; ++i)
      m_str[i] = str[i];
  }

  constexpr FixedString() = default;

  constexpr char &operator[](size_t idx) {
    assert(idx < N && "index out of range");
    return m_str[idx];
  }

  template <std::size_t Idx> constexpr const char &get() const {
    static_assert(Idx < N, "index out of range");
    return m_str[Idx];
  }

  constexpr std::size_t size() const { return std::size(m_str); }
};

template <FixedString Str> constexpr auto operator""_fs() { return Str; }

// 比较字符串
template <std::size_t N1, std::size_t N2>
constexpr bool fs_equal(FixedString<N1> Lhs, FixedString<N2> Rhs) {
  if constexpr (N2 != N1)
    return false;
  for (int i = 0; i < N1 - 1; ++i)
    if (Lhs.m_str[i] != Rhs.m_str[i])
      return false;
  return true;
}

// 拼接字符串
template <std::size_t N1, std::size_t N2>
constexpr auto fs_concat(FixedString<N1> S1, FixedString<N2> S2) {
  FixedString<N1 + N2 - 1> result{};
  for (std::size_t i = 0; i < N1 - 1; ++i) {
    result[i] = S1[i];
  }
  for (std::size_t i = 0; i < N2 - 1; ++i) {
    result[i + N1 - 1] = S2[i];
  }
  result[N1 + N2 - 2] = '\0';
  return result;
}

// 查找字符对应下标
struct FindResult {
  std::size_t index;
  bool success;
};
template <std::size_t N>
constexpr FindResult fs_find(FixedString<N> Str, char c) {
  for (std::size_t i = 0; i < N - 1; ++i) {
    if (Str[i] == c) {
      return {.index = i, .success = true};
    }
  }
  return {.index = 0, .success = false};
}

// 子串
template <std::size_t Pos, std::size_t Len, std::size_t N>
constexpr std::optional<FixedString<Len + 1>> fs_substr(FixedString<N> str) {
  if (Pos >= N || Pos + Len > N - 1) {
    return std::nullopt;
  }
  FixedString<Len + 1> result{};
  for (std::size_t i = 0; i < Len; ++i) {
    result[i] = str[Pos + i];
  }
  result[Len] = '\0';
  return result;
}

// 子串视图
template <std::size_t N>
std::string_view fs_substr_view(FixedString<N> str, std::size_t pos,
                                std::size_t len) {
  return std::string_view{str.m_str + pos, len};
}

// 串式图
template <std::size_t N> std::string_view fs_string_view(FixedString<N> str) {
  return std::string_view{str.m_str};
}

struct SqlNull {};

struct SqlNow {};

// 类型标签
struct sql_integer {};

struct sql_floating {};

struct sql_text {};

struct sql_boolean {};

struct sql_null {};

struct sql_expr {};

// 类型分派标签
namespace detail {
// sql 类型比较
template <typename Column, typename Value>
struct sql_compatible_impl : std::false_type {};

template <>
struct sql_compatible_impl<sql_boolean, sql_boolean> : std::true_type {};

template <>
struct sql_compatible_impl<sql_integer, sql_integer> : std::true_type {};

template <>
struct sql_compatible_impl<sql_floating, sql_floating> : std::true_type {};

template <> struct sql_compatible_impl<sql_text, sql_text> : std::true_type {};

template <> struct sql_compatible_impl<sql_null, sql_null> : std::true_type {};

// 获取 cpp 对应的 sql 类型
// 兜底
template <typename T> struct cpp_type_to_sql_semantic_impl {
  static_assert(sizeof(T) == 0, "\ncpp_type_to_sql_semantic: 不支持的类型\n");
};

// boolean
template <> struct cpp_type_to_sql_semantic_impl<bool> {
  using type = sql_boolean;
};

// integer
template <std::integral T> struct cpp_type_to_sql_semantic_impl<T> {
  using type = sql_integer;
};

// floating point
template <std::floating_point T> struct cpp_type_to_sql_semantic_impl<T> {
  using type = sql_floating;
};

// string
template <std::size_t N> struct cpp_type_to_sql_semantic_impl<FixedString<N>> {
  using type = sql_text;
};

template <> struct cpp_type_to_sql_semantic_impl<std::string> {
  using type = sql_text;
};

template <> struct cpp_type_to_sql_semantic_impl<const char *> {
  using type = sql_text;
};

// null
template <> struct cpp_type_to_sql_semantic_impl<SqlNull> {
  using type = sql_null;
};

// enum: enum -> integer
template <typename T>
  requires std::is_enum_v<T>
struct cpp_type_to_sql_semantic_impl<T> {
  using type = sql_integer;
};
} // namespace detail

template <typename> struct sql_value_from_type {};

template <> struct sql_value_from_type<bool> {
  using type = sql_boolean;
};

// enum 使用的值语义
template <typename T>
  requires std::is_enum_v<T>
struct sql_value_from_type<T> {
  using type = sql_integer;
};

template <std::integral T> struct sql_value_from_type<T> {
  using type = sql_integer;
};

template <std::floating_point T> struct sql_value_from_type<T> {
  using type = sql_floating;
};

template <std::size_t N> struct sql_value_from_type<meta::FixedString<N>> {
  using type = sql_text;
};

template <> struct sql_value_from_type<SqlNull> {
  using type = sql_null;
};

template <auto Value> struct sql_value_tag {
  using type =
      typename sql_value_from_type<std::remove_cvref_t<decltype(Value)>>::type;
};

// 检查对应sql类型是否相同
template <typename Column, typename Value>
struct sql_compatible : detail::sql_compatible_impl<Column, Value> {};

template <typename Column, typename Value>
inline constexpr bool sql_compatible_v =
    sql_compatible<std::remove_cvref_t<Column>,
                   std::remove_cvref_t<Value>>::value;

// 获取 cpp 对应的 sql 类型
template <typename T>
struct cpp_type_to_sql_semantic
    : detail::cpp_type_to_sql_semantic_impl<std::remove_cvref_t<T>> {};

template <typename T>
using cpp_type_to_sql_semantic_t = cpp_type_to_sql_semantic<T>::type;
} // namespace ess::orm::meta
