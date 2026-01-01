#pragma once
#include <core.hpp>

namespace ess::orm::concepts {
template <auto Ptr>
concept not_null_pointer = (Ptr != nullptr);

template <typename T>
concept is_sql_literal_type =
    std::is_integral_v<T> || std::is_floating_point_v<T> ||
    std::is_same_v<T, bool> || std::is_enum_v<T>;
} // namespace ess::orm::concepts
