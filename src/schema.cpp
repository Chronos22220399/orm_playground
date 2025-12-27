#include <array>
#include <cassert>
#include <fmt/color.h>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <thread_db.h>
#include <utility>

namespace ess {
template <size_t N> struct FixedString {
  char m_str[N];

  constexpr FixedString(const char (&str)[N]) {
    for (int i = 0; i < N; ++i)
      m_str[i] = str[i];
  }

  constexpr char &operator[](size_t idx) {
    static_assert(idx >= N, "index out of range");
    return m_str[idx];
  }

  constexpr size_t size() const { return m_str.size(); }
};

namespace orm {

template <FixedString ColumnName, typename... Attrs> struct Filed {
  static constexpr FixedString column_name = ColumnName;
  static constexpr std::tuple<Attrs...> attributes{};
};

template <typename T> struct is_filed : std::false_type {};

template <FixedString FiledName, typename... Attrs>
struct is_filed<Filed<FiledName, Attrs...>> : std::true_type {};

template <typename T>
concept FiledType = is_filed<T>::value;

template <FixedString TableName, FiledType... Fileds> struct Schema {
  static constexpr FixedString table_name = TableName;
  static constexpr std::tuple<Fileds...> fileds{};
};

} // namespace orm

} // namespace ess

struct Inventory {
  using Schema = ess::orm::Schema<"inventory", ess::orm::Filed<"price">>;
};

template <size_t N> void println(const ess::FixedString<N> &str) {
  fmt::println("{}", std::string_view(str.m_str, N - 1));
}

int main() {
  println(Inventory::Schema::table_name);
  // int x = 10;
  // auto func = [&x]() { x++; };
}
