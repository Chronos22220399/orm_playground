#include <core.hpp>

using namespace std;
class Solution {
public:
  vector<int> productExceptSelf(vector<int> &nums) {
    int n = nums.size();
    vector<int> left(n, 1), right(n, 1);
    vector<int> ret(n, 0);
    // get prefix and suffix
    left[0] = nums[0];
    right[n - 1] = nums[n - 1];
    for (int i = 1; i < n; ++i)
      left[i] = left[i - 1] * nums[i];
    for (int i = n - 2; i > -1; --i)
      right[i] = right[i + 1] * nums[i];

    for (int i = 0; i < nums.size(); ++i)
      if (i == 0)
        ret[i] = right[i + 1];
      else if (i == nums.size() - 1)
        ret[i] = left[i - 1];
      else
        ret[i] = left[i - 1] * right[i + 1];

    return ret;
  }
};
