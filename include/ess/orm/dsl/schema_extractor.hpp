#pragma once
#include <ess/orm/dsl/traits.hpp>
#include <tuple>

namespace ess::orm::dsl {

/*
 * @author: Ess
 */
template <typename Schema> class SchemaExtractor {
  template <typename Attrs> static constexpr int find_serialized_name_idx() {
    int found = -1;
    [&found]<std::size_t... I>(std::index_sequence<I...>) {
      using namespace attribute::detail;
      ((found == -1 && std::is_same_v<serialized_name_tag,
                                      typename attribute_category<
                                          std::tuple_element_t<I, Attrs>>::type>
            ? (found = I)
            : 0),
       ...);
    }(std::make_index_sequence<std::tuple_size_v<Attrs>>{});
    return found;
  }

  template <typename Attrs> static constexpr auto get_serialized_name() {
    using namespace meta;
    constexpr int sname_idx = find_serialized_name_idx<Attrs>();
    if constexpr (sname_idx != -1) {
      return std::tuple_element_t<sname_idx, Attrs>::name;
    } else {
      return ""_fs;
    }
  }

  template <typename Attrs>
  static constexpr bool has_serialized_name_v = []() {
    if constexpr (std::tuple_size_v<Attrs> == 0)
      return false;

    constexpr int idx = find_serialized_name_idx<Attrs>();
    if constexpr (idx == -1) {
      return false;
    } else {
      return !std::tuple_element_t<idx, Attrs>::name.empty();
    }
  }();

public:
  using fields = Schema::fields;
  static constexpr auto table_name = Schema::table_name;
  static constexpr size_t field_cnt = std::tuple_size_v<fields>;

  template <size_t Idx> struct FieldInfo {
    using field_type = std::tuple_element_t<Idx, fields>;
    using pointer_type = field_type::pointer_type;
    using member_type = field_type::member_type;
    using attributes = field_type::attributes;

    static constexpr auto column_name = field_type::column_name;
    static constexpr auto pointer = field_type::pointer;

    static constexpr bool has_serialized_name =
        has_serialized_name_v<attributes>;
    static constexpr auto serialized_name = get_serialized_name<attributes>();

    static constexpr size_t index = Idx;
  };
};
} // namespace ess::orm::dsl
