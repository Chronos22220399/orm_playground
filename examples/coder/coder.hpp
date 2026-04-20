#include "./json.hpp"
#include <ess/orm/dsl/schema_extractor.hpp>
#include <ess/orm/dsl/visitor.hpp>

class Coder final {
public:
  template <ess::orm::concepts::table_type Table>
  static nlohmann::json to_json(Table &object) {
    using namespace ess::orm;
    nlohmann::json ret;
    dsl::Visitor<Table>{}.on(object).foreach ([&ret](auto info, auto &value) {
      if constexpr (info.has_serialized_name) {
        ret[info.serialized_name.str()] = value;
      } else {
        ret[info.column_name.str()] = value;
      }
    });
    return ret;
  }

  template <ess::orm::concepts::table_type Table>
  static void from_json(nlohmann::json &json, Table &object) {
    using namespace ess::orm;
    dsl::Visitor<Table>{}.on(object).foreach ([&json](auto info, auto &value) {
      if constexpr (info.has_serialized_name) {
        value = json[info.serialized_name.str()];
      } else {
        value = json[info.column_name.str()];
      }
    });
  }
};
