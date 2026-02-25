#pragma once
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/connection_pool.hpp>
#include <ess/orm/core/transaction.hpp>
#include <optional>

namespace ess::orm {

namespace impl {
template <concepts::database_type DB> struct TxState {
  static thread_local core::Connection *active_conn;
  static thread_local core::TxMode active_mode;
};

template <concepts::database_type DB>
thread_local core::Connection *TxState<DB>::active_conn = nullptr;

template <concepts::database_type DB>
thread_local core::TxMode TxState<DB>::active_mode = core::TxMode::WRITE;

} // namespace impl

template <typename Func, typename Mode, typename DB>
concept tx_callback_func = std::invocable<Func, core::Transaction<Mode, DB> &>;

// 显式模式
template <core::transaction_mode Mode = core::Write,
          concepts::database_type DB = config::default_db, typename Func>
  requires tx_callback_func<Func, Mode, DB>
auto transaction(Func &&func) {
  auto *&active = impl::TxState<DB>::active_conn;
  bool is_root = active == nullptr;

  if constexpr (std::is_same_v<Mode, core::Write>) {
    if (!is_root && impl::TxState<DB>::active_mode == core::TxMode::READ) {
      throw std::runtime_error(
          get_cur_loc_info() +
          ": Cannot nest write transaction inside read transaction");
    }
  }

  std::optional<typename core::ConnectionPool<DB>::Loan> loan = std::nullopt;
  if (is_root) {
    loan.emplace();
  }

  // Transaction<Mode, DB> tx =
  //     Transaction<Mode, DB>(conn_ptr, current_ctx->nesting_level());
  //
  // tx.begin();
  // try {
  //   using ret_type = std::invoke_result_t<Func, Transaction<Mode, DB> &>;
  //   if constexpr (std::is_void_v<ret_type>) {
  //     std::invoke(std::forward<Func>(func), tx);
  //     tx.commit();
  //     return;
  //   } else {
  //     auto result = std::invoke(std::forward<Func>(func), tx);
  //     tx.commit();
  //     return result;
  //   }
  // } catch (...) {
  //   tx.rollback();
  //   // TODO: 后续可通过在错误中添加对数据库类型的比较实现彻底的数据库事务隔离
  //   //
  //   当前的实现下，嵌套的数据库A的事务rollback后，抛出的错误会直接影响到外层的事务，会让外层随之rollback，后续可按照todo的更改
  //   throw;
  // }
}

} // namespace ess::orm
