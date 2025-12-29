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

template <typename T> struct ElemType {};

template <typename T> struct ElemType<std::vector<T>> {
  using value = T;
};

template <typename T> struct ElemType<std::deque<T>> {
  using value = T;
};

template <typename T> struct RemoveReferenceT {
  using type = T;
};

template <typename T> struct RemoveReferenceT<T &> {
  using type = T;
};

template <typename T> struct RemoveReferenceT<T *> {
  using type = T;
};

int main() {
  int nums[] = {1, 2, 3, 4, 5};
  fmt::println("{}", accum(nums, nums + 5));
  fmt::println("{}", accum<int, MutPolicy>(nums, nums + 5));
  fmt::println("{}", TypeSize<decltype(nums)>::value);
}
