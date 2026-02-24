#include "sqlite3_connection.h"
#include <ess/orm/core/conn_factory.h>

namespace ess::orm::core {

ConnectionPtr ConnFactory<dialect::Sqlite3>::create(std::string_view conn_url) {
  return ConnectionPtr(new sqlite3_impl::Sqlite3Connection(conn_url));
}

} // namespace ess::orm::core
