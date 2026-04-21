#pragma once
#include <ess/orm/common/error.hpp>
#include <ess/orm/core/context.hpp>
#include <ess/orm/core/defines.hpp>
#include <ess/orm/core/result.hpp>
#include <ess/orm/core/result_set_mapper.hpp>
#include <ess/orm/sql/lexer.hpp>

namespace ess::orm::impl {
template <core::table_wrapper_type TableTag,
          template <typename...> class Container,
          typename ContainerSize =
              core::ContainerSize<config::default_container_size>>
inline auto select_impl(core::Statement &stmt) {

  if constexpr (std::is_same_v<typename TableTag::type, void>) {
    Container<Row> rows;

    if constexpr (requires(Container<Row> &c) {
                    c.reserve(ContainerSize::value);
                  }) {
      rows.reserve(ContainerSize::value);
    }

    ResultSetMapper<Row> mapper;
    while (stmt.next()) {
      Row row = mapper.map_row(stmt);
      if constexpr (requires(Container<Row> &c) {
                      c.emplace_back(std::move(row));
                    }) {
        rows.emplace_back(std::move(row));
      } else {
        rows.push_back(std::move(row));
      }
    }
    return rows;
  } else {
    using Table = TableTag::type;

    Container<Table> rows;

    if constexpr (requires(Container<Table> &c) {
                    c.reserve(ContainerSize::value);
                  }) {
      rows.reserve(ContainerSize::value);
    }

    ResultSetMapper<Table> mapper;
    mapper.init_mapper(stmt);
    while (stmt.next()) {
      Table obj = mapper.map_row(stmt);
      if constexpr (requires(Container<Table> &c) {
                      c.emplace_back(std::move(obj));
                    }) {
        rows.emplace_back(std::move(obj));
      } else {
        rows.push_back(std::move(obj));
      }
    }
    return rows;
  }
}

/*
 */
inline InsertResult insert_impl(core::Connection &conn, core::Statement &stmt) {
  stmt.next();
  return InsertResult{.last_insert_id = conn.last_insert_id(),
                      .affected_rows = conn.affected_rows()};
}

/*
 */
inline ModifyResult update_impl(core::Connection &conn, core::Statement &stmt) {
  stmt.next();
  return ModifyResult{.affected_rows = conn.affected_rows()};
}

/*
 */
inline ModifyResult delete_impl(core::Connection &conn, core::Statement &stmt) {
  stmt.next();
  return ModifyResult{.affected_rows = conn.affected_rows()};
}

/*
 * @author Ess
 * no table
 */
template <core::table_wrapper_type TableTag, meta::FixedString SQL,
          concepts::database_type DB,
          template <typename...> class Container = std::vector,
          typename ContainerSize =
              core::ContainerSize<config::default_container_size>,
          typename... Args>
auto query_impl(core::conn_ptr_wrapper wrapper, Args &&...args) {
  using namespace ess::orm::core;
  using namespace ess::orm::sql;

  constexpr auto tokens = Lexer(SQL).template tokenize<128>().tokens;
  constexpr TokenType first_token = tokens[0].type;
  constexpr TokenType query_type =
      (first_token == TokenType::With) ? TokenType::Select : first_token;

  core::Connection *conn;
  if (wrapper.m_value == nullptr) {
    auto loan = Context::instance().conn_pool<DB>().acquire();
    conn = loan.get();
  } else {
    conn = wrapper.m_value;
  }

  auto &stmt = conn->prepare_cached(SQL);
  auto guard = stmt.scope_guard();

  if (sizeof...(args) > 0) {
    // MARK: debug 模式
    // std::cout << "binding params: ";
    // ((std::cout << args << " "), ...);
    // std::cout << "\n";

    stmt.bind_params(std::forward<Args>(args)...);
  }

  if constexpr (query_type == TokenType::Select) {
    return select_impl<TableTag, Container, ContainerSize>(stmt);
  } else if constexpr (query_type == TokenType::Insert) {
    return insert_impl(*conn, stmt);
  } else if constexpr (query_type == TokenType::Update) {
    return update_impl(*conn, stmt);
  } else if constexpr (query_type == TokenType::Delete) {
    return delete_impl(*conn, stmt);
  } else {
  }
}

} // namespace ess::orm::impl
