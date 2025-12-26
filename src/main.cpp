#include <deque>
#include <fmt/color.h>
#include <fmt/format.h>
#include <type_traits>

// template parameters

template <typename T> constexpr T pi{};

template <typename T> class MyClass {
public:
  static constexpr int max = 1000;
};

template <typename T> int myMax = MyClass<T>::max;

// template container
template <typename T, template <typename, typename> class Cont = std::deque>
class Stack {
  Cont<T, std::allocator<T>> data;
};

template <typename T, typename Allocator = std::allocator<T>> class MyCont {};

int main() { auto s = Stack<int, MyCont>(); }
