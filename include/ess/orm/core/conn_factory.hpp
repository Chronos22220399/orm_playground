#pragma once
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/dialect.hpp>
#include <string_view>

namespace ess::orm::core {
template <typename dialect> struct ESS_ORM_API ConnFactory;

template <> struct ESS_ORM_API ConnFactory<dialect::Sqlite3> {
  [[nodiscard]] static ConnectionPtr create(std::string_view conn_url);
};

template <> struct ESS_ORM_API ConnFactory<dialect::Mysql> {
  [[nodiscard]] static ConnectionPtr create(std::string_view conn_url,
                                            std::string_view passwd);
};

template <> struct ESS_ORM_API ConnFactory<dialect::Postgres> {
  [[nodiscard]] static ConnectionPtr create(std::string_view conn_url,
                                            std::string_view passwd);
};
} // namespace ess::orm::core
