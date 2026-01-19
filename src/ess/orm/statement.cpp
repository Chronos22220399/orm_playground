#include <std.hpp>
#include <ess/orm/connection.h>
#include <ess/orm/statement.h>

namespace ess::orm {

Statement::Statement(std::shared_ptr<Connection> conn, std::string_view sql)
    : m_conn_ref(conn) {
  if (auto _conn = m_conn_ref.lock()) {
    prepare(_conn->handle(), sql);
    return;
  }
  throw std::runtime_error("Database connection lost");
}

sqlite3 *Statement::get_db_handle() const {
  if (auto conn = m_conn_ref.lock()) {
    return conn->handle();
  }
  throw std::runtime_error("Database connection lost");
}
} // namespace ess::orm
