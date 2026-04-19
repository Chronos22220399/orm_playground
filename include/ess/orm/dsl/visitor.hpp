#pragma once
#include <ess/orm/common/concept.hpp>
#include <ess/orm/dsl/accessor.hpp>
#include <type_traits>

namespace ess::orm::dsl {

template <concepts::table_type Table> struct TableBinding {
  Table &m_instance;

  TableBinding(Table &instance) : m_instance(instance) {}
};

/*
 * @author: Ess
 * accessor: visit the table
 */
template <concepts::table_type Table> class Visitor {
public:
  auto on(Table &instance) { return TableBinding<Table>(instance); }

  static void foreach (auto &&func) {
    using extractor = SchemaExtractor<typename Table::Schema>;
    [&]<size_t... I>(std::index_sequence<I...>) {
      ((func(typename extractor::template FieldInfo<I>{})), ...);
    }(std::make_index_sequence<extractor::field_cnt>{});
  }
};

} // namespace ess::orm::dsl
