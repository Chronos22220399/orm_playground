#pragma once
#include <chrono>
#include <deque>
#include <ess/orm/core/conn_factory.h>
#include <ess/orm/core/connection.h>
#include <ess/orm/defines.h>
#include <ess/orm/error.hpp>

namespace ess::orm::core {

template <dialect::dialect_type Dialect>
class ESS_ORM_API ConnectionPool
    : public std::enable_shared_from_this<ConnectionPool<Dialect>> {
public:
  [[nodiscard]] static std::shared_ptr<ConnectionPool>
  create(std::string_view conn_url, std::size_t pool_size) {
    return std::shared_ptr<ConnectionPool>(
        new ConnectionPool(conn_url, pool_size));
  }

  ~ConnectionPool() {
    m_shutdown.store(true);
    m_cv.notify_all();
  }

  class Loan {
    std::shared_ptr<ConnectionPool> m_pool_ref;
    ConnectionPtr m_conn;

  public:
    Loan(std::shared_ptr<ConnectionPool> pool, ConnectionPtr conn)
        : m_pool_ref(std::move(pool)), m_conn(std::move(conn)) {}

    ~Loan() {
      if (m_pool_ref && m_conn) {
        m_pool_ref->release(std::move(m_conn));
      }
    }

    Loan(Loan &&) noexcept = default;
    Loan &operator=(Loan &&) noexcept = default;
    Loan(const Loan &) = delete;
    Loan &operator=(const Loan &) = delete;

    Connection *operator->() const { return m_conn.get(); }
    Connection &operator*() const { return *m_conn.get(); }
  };

  [[nodiscard]] Loan acquire() {
    std::unique_lock lock(m_mutex);
    bool got = m_cv.wait_for(lock, std::chrono::seconds{5}, [this]() {
      return !m_pool.empty() || !m_shutdown;
    });

    if (m_shutdown)
      throw std::runtime_error(get_cur_loc_info() + ": Pool is shutting down");
    if (!got || m_pool.empty())
      throw std::runtime_error(get_cur_loc_info() +
                               ": Acquire connection timeout");
    auto conn = std::move(m_pool.front());
    m_pool.pop_front();
    return Loan(this->shared_from_this(), std::move(conn));
  }

  size_t available() const {
    std::lock_guard lock(m_mutex);
    return m_pool.size();
  }

private:
  ConnectionPool(std::string_view connection_url, size_t pool_size)
      : m_connection_url(connection_url), m_pool_size(pool_size),
        m_shutdown(false) {
    for (int i = 0; i < m_pool_size; ++i) {
      m_pool.push_back(ConnFactory<Dialect>::create(m_connection_url));
    }
  }

  void release(ConnectionPtr conn) {
    if (m_shutdown)
      return;
    std::lock_guard lock(m_mutex);
    m_pool.push_back(std::move(conn));
    m_cv.notify_one();
  }

private:
  std::deque<ConnectionPtr> m_pool;
  mutable std::mutex m_mutex;
  mutable std::condition_variable m_cv;
  std::string m_connection_url;
  std::size_t m_pool_size;
  std::atomic<bool> m_shutdown;
};

} // namespace ess::orm::core
