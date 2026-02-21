#pragma once
#include <memory_resource>
#include <source_location>
#include <string_view>

namespace ess::orm {

template <typename... Args>
inline void
throw_error(Args &&...args,
            std::source_location const &loc = std::source_location::current()) {
  char buffer[512];
  std::pmr::monotonic_buffer_resource res(buffer, sizeof(buffer),
                                          std::pmr::null_memory_resource());

  std::pmr::string err_msg("[Orm Error]: ", &res);
}

} // namespace ess::orm
