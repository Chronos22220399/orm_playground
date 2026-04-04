#pragma once
#include <cassert>
#include <string_view>
#include <vector>

namespace ess::orm::meta {
template <std::size_t N> struct FixedString {
  char value[N];

  constexpr FixedString(const char (&str)[N]) {
    for (int i = 0; i < N; ++i)
      value[i] = str[i];
  }

  constexpr FixedString(const FixedString<N> &s) {
    for (int i = 0; i < N; ++i)
      value[i] = s[i];
  }

  template <std::size_t N1> constexpr FixedString(const FixedString<N1> &s) {
    static_assert(N1 <= N,
                  "FixedString can only accept a equal length or shorter fs");
    for (int i = 0; i < N1; ++i)
      value[i] = s[i];
    for (int i = N1; i < N; ++i)
      value[i] = '\0';
  }

  constexpr FixedString() {
    for (int i = 0; i < N; ++i)
      value[i] = '\0';
  };

  constexpr FixedString<N> &operator=(const char *str) {
    for (int i = 0; str[i] != '\0'; ++i)
      value[i] = str[i];
    return *this;
  }

  template <std::size_t N1> constexpr bool operator==(FixedString<N1> other) {
    if constexpr (N1 != N)
      return false;

    for (int i = 0; i < N; ++i) {
      if (value[i] != other[i])
        return false;
    }
    return true;
  }

  constexpr char &operator[](size_t idx) {
    assert(idx < N && "index out of range");
    return value[idx];
  }

  constexpr char operator[](size_t idx) const {
    assert(idx < N && "index out of range");
    return value[idx];
  }

  template <std::size_t Idx> constexpr const char &get() const {
    static_assert(Idx < N, "index out of range");
    return value[Idx];
  }

  constexpr std::size_t size() const { return std::size(value); }

  constexpr const char *data() const { return value; }

  constexpr operator std::string_view() const {
    return std::string_view{value, N - 1};
  }

  constexpr bool empty() const { return N == 1; }
};

template <FixedString Str> constexpr auto operator""_fs() { return Str; }

template <std::floating_point T> struct FloatingPoint {
  T value;
  constexpr FloatingPoint(T v) : value(v) {}
  constexpr operator T() const { return value; }
};

template <std::floating_point T> FloatingPoint(T) -> FloatingPoint<T>;

constexpr auto operator""_fp(long double v) {
  return FloatingPoint{static_cast<double>(v)};
}

constexpr auto operator""_fp(unsigned long long v) {
  return FloatingPoint{static_cast<double>(v)};
}

constexpr auto operator""_fpf(long double v) {
  return FloatingPoint{static_cast<float>(v)};
}

constexpr auto operator""_fpf(unsigned long long v) {
  return FloatingPoint{static_cast<float>(v)};
}

// 比较字符串
template <std::size_t N1, std::size_t N2>
constexpr bool fs_equal(FixedString<N1> lhs, FixedString<N2> rhs) {
  if constexpr (N2 != N1)
    return false;
  for (int i = 0; i < N1 - 1; ++i)
    if (lhs.value[i] != rhs.value[i])
      return false;
  return true;
}

// 拼接字符串
template <std::size_t N1, std::size_t N2>
constexpr auto fs_concat(FixedString<N1> str_1, FixedString<N2> str_2) {
  FixedString<N1 + N2 - 1> result{};
  for (std::size_t i = 0; i < N1 - 1; ++i) {
    result[i] = str_1[i];
  }
  for (std::size_t i = 0; i < N2 - 1; ++i) {
    result[i + N1 - 1] = str_2[i];
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
constexpr FindResult fs_find(FixedString<N> str, char c) {
  for (std::size_t i = 0; i < N - 1; ++i) {
    if (str[i] == c) {
      return {.index = i, .success = true};
    }
  }
  return {.index = 0, .success = false};
}

// 子串
template <std::size_t Pos, std::size_t Len, std::size_t N>
constexpr FixedString<Len + 1> fs_substr(FixedString<N> str) {
  static_assert(Pos < N, "Substring position out of range");
  static_assert(Pos + Len < N, "Substring out of range");

  FixedString<Len + 1> result{};
  for (std::size_t i = 0; i < Len; ++i) {
    result[i] = str[Pos + i];
  }
  result[Len] = '\0';
  return result;
}

// 串视图
template <std::size_t N>
constexpr std::string_view fs_string_view(FixedString<N> str) {
  return std::string_view{str.value()};
}

// 判断是否以prefix为前缀
template <std::size_t N1, std::size_t N2>
constexpr bool fs_starts_with(FixedString<N1> str, FixedString<N2> prefix) {
  if constexpr (N1 < N2) {
    return false;
  }
  for (int i = 0; i < N2 - 1; ++i) {
    if (str[i] != prefix[i]) {
      return false;
    }
  }
  return true;
}

// 改为大写
template <std::size_t N> constexpr auto fs_to_upper(FixedString<N> str) {
  FixedString<N> result{};
  for (int i = 0; i < N - 1; ++i) {
    if (str[i] >= 'a' && str[i] <= 'z') {
      result[i] = str[i] - 32;
    } else {
      result[i] = str[i];
    }
  }
  return result;
}

// 改为小写
template <std::size_t N> constexpr auto str_to_lower(FixedString<N> str) {
  FixedString<N> result{};
  for (int i = 0; i < N - 1; ++i) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      result[i] = str[i] + 32;
    } else {
      result[i] = str[i];
    }
  }
  return result;
}

// 将字符转为大写
constexpr char to_upper(char c) {
  if (c >= 'a' && c <= 'z')
    return c - 32;
  return c;
}

// 将字符转为小写
constexpr char to_lower(char c) {
  if (c >= 'A' && c <= 'Z')
    return static_cast<char>(c + 32);
  return c;
}

// 判断是否为空白字符
constexpr bool is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
         c == '\v';
}

// 忽略大小写的比较
template <std::size_t N1, std::size_t N2>
consteval bool fs_equal_ignore_case(FixedString<N1> lhs, FixedString<N2> rhs) {
  if constexpr (N1 != N2) {
    return false;
  }
  for (int i = 0; i < N1 - 1; ++i) {
    if (to_lower(lhs[i]) != to_lower(rhs[i])) {
      return false;
    }
  }
  return true;
}

// 寻找第一个非空白字符的下标
template <std::size_t N>
constexpr FindResult fs_find_first_non_space(FixedString<N> str) {
  for (std::size_t i = 0; i < N - 1; ++i) {
    if (!is_space(str[i]))
      return {.index = i, .success = true};
  }
  return {.index = N, .success = false};
}

// 寻找最后一个非空白字符的下标
template <std::size_t N>
consteval FindResult fs_find_last_non_space(FixedString<N> str) {
  std::size_t i = N - 1;
  while (i > 0) {
    i--;
    if (!is_space(str[i]))
      return {.index = (std::size_t)i, .success = true};
  }
  return {.index = N, .success = false};
}

template <FixedString Str> consteval auto fs_trim() {
  constexpr auto begin_res = fs_find_first_non_space(Str);
  constexpr auto end_res = fs_find_last_non_space(Str);

  constexpr bool is_empty_or_blank = (!begin_res.success || !end_res.success ||
                                      (begin_res.index > end_res.index));

  if constexpr (is_empty_or_blank) {
    return FixedString<1>{""};
  } else {
    constexpr auto len = end_res.index - begin_res.index + 1;
    return fs_substr<begin_res.index, len>(Str).value();
  }
}

template <std::size_t N1, std::size_t N2>
constexpr bool fs_compare_ignore_len(FixedString<N1> lhs, FixedString<N2> rhs) {
  std::size_t len = N1 > N2 ? N2 : N1;
  for (int i = 0; i < len; ++i) {
    if (lhs[i] != rhs[i])
      return false;
  }
  return true;
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

template <std::floating_point T>
struct sql_value_from_type<meta::FloatingPoint<T>> {
  using type = sql_floating;
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

template <typename> struct sql_semantic_to_type_str {};

template <> struct sql_semantic_to_type_str<sql_integer> {
  static constexpr auto type_str = "INT"_fs;
};

template <> struct sql_semantic_to_type_str<sql_floating> {
  static constexpr auto type_str = "REAL"_fs;
};

template <> struct sql_semantic_to_type_str<sql_text> {
  static constexpr auto type_str = "TEXT"_fs;
};

template <> struct sql_semantic_to_type_str<sql_boolean> {
  static constexpr auto type_str = "BOOLEAN"_fs;
};

template <std::size_t N>
constexpr std::string_view format_as(const FixedString<N> &str) {
  return std::string_view(str);
}

// join
static std::string join(const std::vector<std::string> &elements,
                        std::string_view delimeter) {
  if (elements.empty())
    return "";
  std::size_t total_len = 0;
  std::string result{};
  // 计算总长度
  for (auto &elem : elements) {
    total_len += elem.size();
  }
  total_len += delimeter.size() * (elements.size() - 1);
  // 预留位置并插入
  result.reserve(total_len);
  for (std::size_t i = 0; i < elements.size(); ++i) {
    result += elements[i];
    if (i < elements.size() - 1) {
      result += delimeter;
    }
  }
  return result;
}

enum class ColumnType : std::uint8_t {
  Unknown = 0, // 1. 无效或空值
  Null,

  Boolean,
  Int64,

  Float,

  Text,
  Blob,

  DateTime, // 对应 TIMESTAMP / DATETIME
  Date      // 对应 DATE
};

} // namespace ess::orm::meta
