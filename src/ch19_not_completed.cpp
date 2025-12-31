#include <core.hpp>

template <typename T> struct AccumulateTraits;

template <> struct AccumulateTraits<char> {
  using AccT = int;
  inline static AccT const zero = 0;
};
template <> struct AccumulateTraits<short> {
  using AccT = int;
  inline static AccT const zero = 0;
};
template <> struct AccumulateTraits<int> {
  using AccT = long;
  inline static AccT const zero = 0;
};
template <> struct AccumulateTraits<unsigned int> {
  using AccT = unsigned long;
  inline static AccT const zero = 0;
};
template <> struct AccumulateTraits<float> {
  using AccT = double;
  inline static AccT const zero = 0;
};

template <typename T1, typename T2> class SumPolicy {
public:
  static void accumulate(T1 &total, T2 const &value) { total += value; }
};

template <typename T, template <typename, typename> class Policy = SumPolicy,
          typename AT = AccumulateTraits<T>>
auto accum(T const *beg, T const *end) {
  ESS_FUNC_LOG();
  using AccT = typename AT::AccT;
  AccT res{AT::zero};
  while (beg != end) {
    Policy<AccT, AccT>::accumulate(res, *beg);
    ++beg;
  }
  return res;
}

template <typename Iter> auto accum(Iter begin, Iter end) {
  using VT = std::iterator_traits<Iter>::value_type;
  VT total{};
  while (begin != end) {
    total += *begin;
    ++begin;
  }
  return total;
}

template <typename T1, typename T2> struct MutPolicy {
public:
  static void accumulate(T1 &total, T2 const &value) {
    if (total == 0)
      total = 1;
    total *= value;
  }
};

template <typename T> struct TypeSize {
  static std::size_t const value = sizeof(T);
};

// element_type
template <typename T> struct ElemType {};

template <typename T> struct ElemType<std::vector<T>> {
  using value = T;
};

template <typename T> struct ElemType<std::deque<T>> {
  using value = T;
};

// remove_reference
template <typename T> struct RemoveReferenceT {
  using type = T;
};

template <typename T> struct RemoveReferenceT<T &> {
  using type = T;
};

template <typename T> struct RemoveReferenceT<T *> {
  using type = T;
};

// add_reference
template <typename T> struct AddLValueReferenceT {
  using type = T &;
};

template <> struct AddLValueReferenceT<void> {
  using type = void;
};

template <> struct AddLValueReferenceT<void const> {
  using type = void const;
};

template <> struct AddLValueReferenceT<void volatile> {
  using type = void volatile;
};

template <> struct AddLValueReferenceT<void const volatile> {
  using type = void volatile;
};

template <typename T> struct AddRValueReferenceT {
  using type = T &&;
};

template <> struct AddRValueReferenceT<void> {
  using type = void;
};

template <> struct AddRValueReferenceT<void const> {
  using type = void const;
};

template <> struct AddRValueReferenceT<void volatile> {
  using type = void volatile;
};

template <> struct AddRValueReferenceT<void const volatile> {
  using type = void volatile;
};

// remove_const
template <typename T> struct RemoveConstT {
  using type = T;
};

template <typename T> struct RemoveConstT<T const> {
  using type = T;
};

// remove_volatile
template <typename T> struct RemoveVolatileT {
  using type = T;
};

template <typename T> struct RemoveVolatileT<T volatile> {
  using type = T;
};

// remove_const_volatile
template <typename T>
struct RemoveCVT : RemoveConstT<typename RemoveVolatileT<T>::type> {};

// decay
// for trivial type
template <typename T> struct DecayT : RemoveCVT<T> {};

// from array to pointer
template <typename T> struct DecayT<T[]> {
  using type = T *;
};

template <typename T, std::size_t N> struct DecayT<T[N]> {
  using type = T *;
};

// from function to function pointer
template <typename Func, typename... Args> struct DecayT<Func(Args...)> {
  using type = Func (*)(Args...);
};

template <typename Func, typename... Args> struct DecayT<Func(Args..., ...)> {
  using type = Func (*)(Args..., ...);
};

// true/flase_type
template <bool val> struct BoolConstant {
  using type = BoolConstant<val>;
  static constexpr bool value = val;
};

using TrueType = BoolConstant<true>;
using FalseType = BoolConstant<false>;

// is_same
template <typename T1, typename T2> struct IsSameT : FalseType {};

template <typename T> struct IsSameT<T, T> : TrueType {};

void fn() {}

template <typename T> void fooImpl(T, TrueType) {
  fmt::println("foolImpl(T, true) for int called");
}

template <typename T> void fooImpl(T, FalseType) {
  fmt::println("foolImpl(T, false) for int called");
}

template <typename T> void foo(T t) { fooImpl(t, IsSameT<T, int>{}); }

struct A {
  virtual void func() = 0;
};

template <typename T> struct IsDefaultConstructibleT {
private:
  template <typename U, typename = decltype(U())> static char test(void *);
  template <typename> static long test(...);

  static constexpr bool value =
      IsSameT<decltype(test<T>(nullptr)), char>::value;
};

int main() {

  foo(32);
  foo(3.2);

  int nums[] = {1, 2, 3, 4, 5};

  using type = DecayT<decltype(fn)>::type;
  static_assert(std::is_same_v<type, std::add_pointer_t<decltype(fn)>>);

  // fmt::println("{}", accum(nums, nums + 5));
  // fmt::println("{}", accum<int, MutPolicy>(nums, nums + 5));
  // fmt::println("{}", TypeSize<decltype(nums)>::value);
}
