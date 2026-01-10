#include <core.hpp>

struct sql_integer {};
struct sql_floating {};
struct sql_text {};

template <typename Type> constexpr std::string type_to_str() {
  if constexpr (std::is_same_v<Type, sql_integer>) {
    return "INTEGER";
  } else if constexpr (std::is_same_v<Type, sql_floating>) {
    return "REAL";
  } else if constexpr (std::is_same_v<Type, sql_text>) {
    return "TEXT";
  } else {
    throw;
  }
}

template <typename... Types> constexpr auto ColumnJoiner() {
  std::string result{};
  ((result += type_to_str<Types>() + ","), ...);
  if (!result.empty()) {
    result.pop_back();
  }
  return result;
}

int main() {
  fmt::println("{}", ColumnJoiner<sql_floating, sql_integer>());
  return 0;
}
