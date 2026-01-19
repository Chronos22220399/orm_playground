#pragma once
#include <ess/orm/connection.h>

namespace ess::orm {

enum class database : int { sqlite3, postgres };

struct ConnSession {
  std::shared_ptr<Connection> conn;
  database db;
  int transaction_level;
};

class DatabaseContext {
  static thread_local std::stack<ConnSession> session_stack;

public:
  static void push(std::shared_ptr<Connection> conn, database db) {
    session_stack.push(
        {.conn = std::move(conn), .db = db, .transaction_level = 0});
  }

  static void pop() {
    if (!session_stack.empty()) {
      session_stack.pop();
    }
  }

  static ConnSession *current() {
    return (session_stack.empty() ? nullptr : &session_stack.top());
  }
};
} // namespace ess::orm
