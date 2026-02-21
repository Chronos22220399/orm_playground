#include <core.hpp>
#include <coroutine>
#include <sqlite3.h>

struct AwaitTimer {
  bool await_ready() const noexcept { return true; }

  void await_suspend(std::coroutine_handle<> h) {
    std::cout << "[Timer] 模拟异步操作中...\n";
    h.resume(); // 立即恢复，实际开发中这里会交给调度器
  }

  int await_resume() noexcept { return 42; }
};

template <typename T> struct Task {

  struct promise_type {
    // 父协程
    std::coroutine_handle<> continuation;
    T m_value{0};

    struct final_awaiter {
      bool await_ready() noexcept { return false; }

      std::coroutine_handle<>
      await_suspend(std::coroutine_handle<promise_type> h) noexcept {
        if (h.promise().continuation) {
          return h.promise().continuation;
        }
        return std::noop_coroutine();
      }

      void await_resume() noexcept {}
    };

    Task<T> get_return_object() {
      return {std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    std::suspend_always initial_suspend() noexcept { return {}; }

    final_awaiter final_suspend() noexcept { return {}; }

    std::suspend_always yield_value(T val) {
      m_value = val;
      return {};
    }

    void return_value(T val) { m_value = val; } // 配合 co_return

    void unhandled_exception() {}
  };

  std::coroutine_handle<promise_type> handle;

  Task(std::coroutine_handle<promise_type> h) : handle(h) {}

  Task(Task const &) = delete;

  Task &operator=(Task const &) = delete;

  Task(Task &&other) : handle(other.handle) { other.handle = nullptr; }

  Task &operator=(Task &&other) {
    if (this != &other) {
      if (handle)
        handle.destroy();
      handle = other.handle;
      other.handle = nullptr;
    }
    return *this;
  }

  T &value() const { return handle.promise().m_value; }

  void resume() const { handle.resume(); }

  bool done() const { return handle.done(); }

  ~Task() {
    if (handle) {
      handle.destroy();
    }
  }

  auto operator co_await() noexcept {
    struct AwaitTimer {
      std::coroutine_handle<promise_type> h;

      bool await_ready() noexcept { return false; }

      std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller) {
        h.promise().continuation = caller;
        return h;
      }

      T await_resume() noexcept { return h.promise().m_value; }
    };

    return AwaitTimer{handle};
  }
};

Task<int> my_coro() {
  std::cout << "[my_coro] 开始运行\n";
  int res = co_await AwaitTimer(); // 挂起 1
  std::cout << "[my_coro] 拿到 Timer 结果: " << res << "\n";
  co_return res + 8; // 结果变成 50
}

Task<int> tst() {
  std::cout << "[tst] 准备调用 my_coro\n";
  int final_res = co_await my_coro(); // 挂起 2
  std::cout << "[tst] 拿到 my_coro 最终结果: " << final_res << "\n";
  co_return final_res;
}

int main() {
  auto coro = tst();
  coro.resume();
  return 0;
}
