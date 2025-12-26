#include <fmt/color.h>
#include <fmt/format.h>
#include <iostream>
#include <type_traits>
#include <vector>

template <typename T> struct MyClass;

template <typename T, std::size_t N> struct MyClass<T[N]> {
  static void print() { fmt::println("print() for T[{}]", N); }
};

template <typename T, std::size_t N> struct MyClass<T (&)[N]> {
  static void print() { fmt::println("print() for T(&)[{}]", N, "]"); }
};

template <typename T> struct MyClass<T[]> {
  static void print() { fmt::println("print() for T[]"); }
};

template <typename T> struct MyClass<T (&)[]> {
  static void print() { fmt::println("print() for T(&)[]"); }
};

template <typename T> struct MyClass<T *> {
  static void print() { fmt::println("print() for T*"); }
};

template <typename T1, typename T2, typename T3>
void foo(int a1[7], int (&a2)[7], int a3[], int (&x0)[], T1 x1, T2 &x2,
         T3 &&x3) {
  MyClass<decltype(a1)>::print();
  MyClass<decltype(a2)>::print();
  MyClass<decltype(a3)>::print();
  MyClass<decltype(x0)>::print();
  MyClass<decltype(x1)>::print();
  MyClass<decltype(x2)>::print();
  MyClass<decltype(x3)>::print();
}

int main() {
  int a[7];
  MyClass<decltype(a)>::print();
  extern int x[];
  MyClass<decltype(x)>::print();
  foo(a, a, a, x, x, x, x);
}

int x[] = {0, 8, 15};
