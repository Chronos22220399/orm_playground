#include <array>
#include <cassert>
#include <defines.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <iostream>
#include <string>
#include <thread_db.h>
#include <utility>
#include <vector>

#define ESS_FUNC_LOG() (fmt::println("{}", ESS_FUNCTION_SIGNATURE))

template <typename T, unsigned N> std::size_t len(T (&)[N]) {
  ESS_FUNC_LOG();
  return N;
}

template <typename T>
auto len(T const &t) -> decltype((void)(t.size()), T::size_type) {
  ESS_FUNC_LOG();
  return t.size();
}

template <typename T> void len(T &&t) { ESS_FUNC_LOG(); }

struct TestFunc {
  using size_type = std::size_t;
  size_type size() const { return {}; }
};

template <typename T>
auto has_size(int) -> decltype(std::declval<T>().size(), std::true_type{});

template <typename T> auto has_size(T) -> std::false_type;

template <typename T> void print(T &&t) {
  ESS_FUNC_LOG();
  if constexpr (decltype(has_size<T>(std::forward<T>(t)))::value) {
    fmt::println("{}", t.size());
  } else {
    fmt::println("no size");
  }
}

int main() {
  std::vector<int> a;
  fmt::println("{}", typeid(a).name());
  print(10);
}
