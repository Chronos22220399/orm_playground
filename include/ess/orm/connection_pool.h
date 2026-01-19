#pragma once
#include <std.hpp>
#include <ess/orm/connection.h>

namespace ess::orm {

class ConnectionPool {
public:
  class ConnectionGuard {
    friend class ConnectionPool;

    std::shared_ptr<Connection> m_conn;

    ConnectionPool *m_pool;

    ConnectionGuard(std::shared_ptr<Connection> conn, ConnectionPool *pool)
        : m_conn(std::move(conn)), m_pool(pool) {}

  public:
    ConnectionGuard(ConnectionGuard const &) = delete;

    ConnectionGuard &operator=(ConnectionGuard const &) = delete;

    ConnectionGuard(ConnectionGuard &&other) noexcept
        : m_conn(std::move(other.m_conn)), m_pool(other.m_pool) {
      other.m_pool = nullptr;
    }

    ConnectionGuard &operator=(ConnectionGuard &&other) noexcept {
      if (this != &other) {
        release();
        m_conn = std::move(other.m_conn);
        m_pool = other.m_pool;
        other.m_pool = nullptr;
      }
      return *this;
    }

    ~ConnectionGuard() { release(); }

    Connection *operator->() { return m_conn.get(); }

    Connection &operator*() { return *m_conn; }

    Connection *get() { return m_conn.get(); }

    std::shared_ptr<Connection> shared() { return m_conn; }

  private:
    void release() {
      if (m_conn && m_pool) {
        m_pool->return_connection(m_conn);
        m_pool = nullptr;
      }
    }
  };

private:
  std::deque<std::shared_ptr<Connection>> m_pool;
  mutable std::mutex m_mutex;
  mutable std::condition_variable m_cv;
  std::string m_connection_url;
  std::size_t m_pool_size;

public:
  ConnectionPool(std::string_view connection_url, size_t pool_size)
      : m_connection_url(connection_url), m_pool_size(pool_size) {
    for (int i = 0; i < m_pool_size; ++i) {
      m_pool.push_back(Connection::create(m_connection_url));
    }
  }

  ~ConnectionPool() {
    std::unique_lock lock(m_mutex);
    // 等待所有连接归还
    m_cv.wait(lock, [this] { return m_pool.size() == m_pool_size; });
  }

  ConnectionGuard acquire() {
    std::unique_lock lock(m_mutex);
    m_cv.wait(lock, [this] { return !m_pool.empty(); });

    auto conn = std::move(m_pool.front());
    m_pool.pop_front();

    return ConnectionGuard(std::move(conn), this);
  }

  std::shared_ptr<Connection> acquire_raw() {
    std::unique_lock lock(m_mutex);
    m_cv.wait(lock, [this] { return !m_pool.empty(); });

    auto conn = std::move(m_pool.front());
    m_pool.pop_front();
    return conn;
  }

  void return_connection(std::shared_ptr<Connection> conn) {
    std::lock_guard lock(m_mutex);
    m_pool.push_back(std::move(conn));
    m_cv.notify_one();
  }

  size_t available() const {
    std::lock_guard lock(m_mutex);
    return m_pool.size();
  }
};
} // namespace ess::orm
