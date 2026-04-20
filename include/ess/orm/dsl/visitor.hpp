#pragma once
#include <ess/orm/common/concept.hpp>
#include <ess/orm/dsl/accessor.hpp>

namespace ess::orm::dsl {

template <concepts::table_type Table> struct TableBinding;

/*
 * @author: Ess
 * accessor: visit the table
 */
template <concepts::table_type Table> class Visitor {
public:
  auto on(Table &instance) { return TableBinding<Table>{instance}; }

  auto on(const Table &instance) const { return TableBinding<Table>{instance}; }

  // template
  static void foreach (auto &&func) {
    using extractor = SchemaExtractor<typename Table::Schema>;
    [&func]<size_t... I>(std::index_sequence<I...>) {
      ((std::forward<decltype(func)>(func)(
           typename extractor::template FieldInfo<I>{})),
       ...);
    }(std::make_index_sequence<extractor::field_cnt>{});
  }
};

template <concepts::table_type Table> struct TableBinding {
  Table &m_instance;

  TableBinding(Table &instance) : m_instance(instance) {}

  void foreach (auto &&func) const {
    Visitor<Table>::foreach ([&](auto info) {
      constexpr size_t Idx = decltype(info)::index;
      auto &value = FieldAccessor<Table>::template get<Idx>(m_instance);
      std::forward<decltype(func)>(func)(info, value);
    });
  }
};
} // namespace ess::orm::dsl
