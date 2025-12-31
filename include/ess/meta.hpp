#pragma once

namespace ess::meta {
template <size_t N> struct FixedString {
  char m_str[N];

  constexpr FixedString(const char (&str)[N]) {
    for (int i = 0; i < N; ++i)
      m_str[i] = str[i];
  }

  constexpr char &operator[](size_t idx) {
    assert(idx < N && "index out of range");
    return m_str[idx];
  }

  template <std::size_t Idx> constexpr const char &get() const {
    static_assert(Idx < N, "index out of range");
    return m_str[Idx];
  }

  constexpr size_t size() const { return std::size(m_str); }
};

template <FixedString Lhs, FixedString Rhs>
consteval bool fixed_string_is_equal() {
  constexpr auto l_size = Lhs.size();
  constexpr auto r_size = Rhs.size();
  if (l_size != r_size)
    return false;
  for (int i = 0, j = 0; i < l_size, j < r_size; ++i, ++j)
    if (Lhs.m_str[i] != Rhs.m_str[j])
      return false;
  return true;
}

} // namespace ess::meta
