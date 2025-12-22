#include <fmt/format.h>
#include <iostream>
#include <type_traits>
#include <vector>

template <typename T>
concept Comparable = requires(T a, T b) { a < b; };

template <typename T>
concept is_not_pair = requires(T a) { !std::decay(a).first; };

template <typename T> class Stack;

template <typename T>
std::ostream &operator<<(std::ostream &, Stack<T> const &);

template <typename T> class Stack {
  friend std::ostream &operator<< <T>(std::ostream &, Stack<T> const &);
};

Stack(const char *) -> Stack<std::string>;

template <typename T> std::ostream &operator<<(std::ostream &os, Stack<T> &s) {}

template <const char *> class MyClass {};

extern char const s03[] = "hi";
char const sl1[] = "hi";

int main() {
  MyClass<s03> a;
  MyClass<sl1> s;
  static char const m[] = "hi";
  MyClass<m> t;
  return 0;
}
