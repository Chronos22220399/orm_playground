#pragma once
#include <cstdint>

namespace ess::orm {

struct InsertResult {
  int64_t last_insert_id;
  int64_t affected_rows;
};

struct ModifyResult {
  int64_t affected_rows;
};

template <typename R> struct return_type {
  using type = R;
};
} // namespace ess::orm
