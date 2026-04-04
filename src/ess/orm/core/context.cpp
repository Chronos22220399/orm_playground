#include <ess/orm/core/context.hpp>

namespace ess::orm {

Context &Context::instance() {
  static Context ctx;
  return ctx;
}

void Context::init() {
  std::call_once(m_init_flag, [this]() {
    // init database
    using databases = config::databases;
    [this]<std::size_t... I>(std::index_sequence<I...>) {
      (register_db<std::tuple_element_t<I, databases>>(), ...);
    }(std::make_index_sequence<config::database_count>{});
  });
}

} // namespace ess::orm
