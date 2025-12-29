#include <core.hpp>

template <typename T>
concept Comparable = requires(T a, T b) { a < b; };

template <typename T>
concept is_pair = requires(T t) {
  typename T::first_type;
  typename T::second_type;
  { t.first } -> std::same_as<typename T::first_type &>;
  { t.second } -> std::same_as<typename T::second_type &>;
};

template <typename T>
concept is_not_pair = !is_pair<std::decay_t<T>>;

template <is_not_pair T> class Stack;

template <typename T>
std::ostream &operator<<(std::ostream &, Stack<T> const &);

template <is_not_pair T> class Stack {
  friend std::ostream &operator<< <T>(std::ostream &, Stack<T> const &);
};

Stack(const char *) -> Stack<std::string>;

template <typename T> std::ostream &operator<<(std::ostream &os, Stack<T> &s) {}

template <const char *> class MyClass {};

extern char const s03[] = "hi";
char const sl1[] = "hi";
