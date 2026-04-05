#include <ess/orm/core/context.hpp>

namespace ess::orm {

Context &Context::instance() {
  static Context ctx;
  return ctx;
}

} // namespace ess::orm

// User must call init() from their code with their config included
// Or manually call register_db<YourDB>() for each database
