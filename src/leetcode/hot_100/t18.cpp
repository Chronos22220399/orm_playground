#include <core.hpp>

class Solution {
public:
  static const int N = 1e5 + 10;

  int firstMissingPositive(std::vector<int> &nums) {
    int ans = 1;
    std::array<int, N> arr{};

    for (auto &n : nums) {
      if (n < N and n > 0) {
        arr[n]++;
      }
    }

    for (int i = 0; i < N; ++i) {
      if (arr[i]) {
        if (ans >= i) {
          ans = i + 1;
        }
      }
    }

    return ans;
  }
};
