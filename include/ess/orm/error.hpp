#pragma once
#include <format>
#include <memory_resource>
#include <source_location>

namespace ess::orm {

inline std::string get_cur_loc_info(
    std::source_location const &loc = std::source_location::current()) {
  std::string err_msg;
  std::format_to(std::back_inserter(err_msg), "[Orm Error]: {}:{} ({})",
                 loc.file_name(), loc.line(), loc.function_name());

  return std::string(err_msg);
}

} // namespace ess::orm
