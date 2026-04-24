#include "include/ess/orm/dsl/attribute.hpp"
#include "include/ess/orm/dsl/field.hpp"
#include "include/ess/orm/dsl/schema.hpp"
#include <string>

struct Invalid {
  std::string name;
  using Schema =
      dsl::Schema<"invalid",
                  dsl::Field<"name", &Invalid::name, attribute::AutoIncrement>>;
};

int main() { return 0; }
