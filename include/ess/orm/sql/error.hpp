#pragma once
#include <string_view>

namespace ess::orm::sql {

struct SqlError {
  std::string_view err_msg;
  std::size_t pos;
  std::size_t token_idx = 0;
};

} // namespace ess::orm::sql
