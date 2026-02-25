#pragma once
#include <chrono>
#include <ess/orm/connection.hpp>
#include <std.hpp>

namespace ess::orm {

class ConnectionPool {
public:
  ConnectionPool(std::string_view connection_url, size_t pool_size)
      : m_connection_url(connection_url), m_pool_size(pool_size),
        m_shutdown(false) {
    for (int i = 0; i < m_pool_size; ++i) {
      m_pool.push_back(Connection::create(m_connection_url));
    }
  }

  ~ConnectionPool() {
    m_shutdown.store(true);
    m_cv.notify_all();
    std::unique_lock lock(m_mutex);
    m_pool.clear();
  }

  std::shared_ptr<Connection> acquire() {
    std::unique_lock lock(m_mutex);
    if (!m_cv.wait_for(lock, std::chrono::seconds{30},
                       [this] { return !m_pool.empty(); })) {
      throw std::runtime_error("Connection pool exhausted, Deadlock may "
                               "occured or too many concurrent threads.");
    }

    if (m_shutdown.load()) {
      throw std::runtime_error("Connection pool is shutting down.");
    }

    auto conn = std::move(m_pool.front());
    m_pool.pop_front();

    return std::shared_ptr<Connection>(conn.get(), [this, conn](Connection *) {
      // 连接池还未关闭
      if (!m_shutdown.load()) {
        std::lock_guard lock(m_mutex);
        m_pool.push_back(conn);
        m_cv.notify_one();
      }
    });
  }

  size_t available() const {
    std::lock_guard lock(m_mutex);
    return m_pool.size();
  }

private:
  std::deque<std::shared_ptr<Connection>> m_pool;
  mutable std::mutex m_mutex;
  mutable std::condition_variable m_cv;
  std::string m_connection_url;
  std::size_t m_pool_size;
  std::atomic<bool> m_shutdown;
};
} // namespace ess::orm
