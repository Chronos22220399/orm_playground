#pragma once
#include <ess/orm/config/config.hpp>
#include <sqlite3.h>

namespace ess::orm {

class Connection {
  sqlite3 *m_db = nullptr;

public:
  Connection() = default;

  Connection(std::string_view connection_url) {
    if (sqlite3_open_v2(connection_url.data(), &m_db, SQLITE_OPEN_READWRITE,
                        nullptr) != SQLITE_OK) {
      throw std::runtime_error(std::string("Can't open the database: ") +
                               connection_url.data() + sqlite3_errmsg(m_db));
    }
  }

  Connection(Connection const &) = delete;

  Connection(Connection &&) = default;

  // Connection
};

} // namespace ess::orm
