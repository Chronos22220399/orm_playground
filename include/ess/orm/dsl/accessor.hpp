#pragma once
#include <ess/orm/common/concept.hpp>
#include <ess/orm/dsl/schema_extractor.hpp>

namespace ess::orm::dsl {

/*
 * @author: Ess
// accessor: visit the table
 */
template <concepts::table_type Table> class FieldAccessor {
  using extractor = SchemaExtractor<typename Table::Schema>;

public:
  template <size_t Idx> static const auto &get(const Table &instance) {
    using info = typename extractor::template FieldInfo<Idx>;
    return instance.*info::pointer;
  }

  template <size_t Idx> static auto &get(Table &instance) {
    using info = typename extractor::template FieldInfo<Idx>;
    return instance.*info::pointer;
  }

  template <size_t Idx, typename V>
  static void set(Table &instance, V &&value) {
    using info = typename extractor::template FieldInfo<Idx>;
    static_assert(std::is_constructible_v<typename info::member_type, V>,
                  "Cannot construct member type from the provided value");
    instance.*info::pointer = std::forward<V>(value);
  }
};
} // namespace ess::orm::dsl
