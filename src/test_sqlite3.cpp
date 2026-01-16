#include <core.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/meta.hpp>
#include <ess/orm/runtime.hpp>
#include <source_location>
#include <sqlite3.h>
#include <unordered_map>

using namespace ess::orm;
using namespace ess::orm::meta;

class Statement {
  sqlite3_stmt *m_stmt = nullptr;

public:
  Statement(sqlite3 *db, std::string_view sql) {
    if ((sqlite3_prepare_v2(db, sql.data(), -1, &m_stmt, nullptr)) !=
        SQLITE_OK) {
      throw std::runtime_error(sqlite3_errmsg(db));
    }
  };

  Statement(const Statement &) = delete;

  Statement &operator=(const Statement &) = delete;

  Statement(Statement &&other) : m_stmt(other.m_stmt) {
    other.m_stmt = nullptr;
  }

  ~Statement() { sqlite3_finalize(m_stmt); }

  sqlite3_stmt *get() const { return m_stmt; }

  void reset() {
    sqlite3_reset(m_stmt);
    sqlite3_clear_bindings(m_stmt);
  }

  template <typename... Args> void bind_params(Args &&...args) {
    int index = 1;
    ((bind_one(index++, std::forward<Args>(args))), ...);
  }

  void execute() {
    int rc = sqlite3_step(m_stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
      throw std::runtime_error(sqlite3_errmsg(sqlite3_db_handle(m_stmt)));
    }
  }

private:
  void bind_one(int index, int value) {
    sqlite3_bind_int(m_stmt, index, value);
  }

  void bind_one(int index, double value) {
    sqlite3_bind_int(m_stmt, index, value);
  }

  void bind_one(int index, std::string_view value) {
    sqlite3_bind_text(m_stmt, index, value.data(), -1, SQLITE_TRANSIENT);
  }
};

struct Inventory {
  int id;
  std::string name;

  using Schema = dsl::Schema<
      "inventory",
      dsl::Field<"id", &Inventory::id, attribute::AutoIncrement,
                 attribute::PrimaryKey>,
      dsl::Field<"name", &Inventory::name, attribute::DefaultValue<"''"_fs>>>;
};

struct InventoryDto {
  std::string name;
  using Schema =
      dsl::Schema<"inventory", dsl::Field<"name", &InventoryDto::name>>;
};

int main() {
  sqlite3 *db = nullptr;
  int rc = sqlite3_open_v2("data/test.db", &db, SQLITE_OPEN_READWRITE, nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "无法打开数据库: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return rc;
  }

  sqlite3_stmt *stmt;
  auto ddl = Inventory::Schema::make_create_table_ddl();

  // fmt::println("{}", ddl);
  rc = sqlite3_prepare_v2(db, "SELECT name FROM inventory WHERE id = 1", -1,
                          &stmt, nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "select stmt prepare failed: " << sqlite3_errmsg(db)
              << std::endl;
    sqlite3_close(db);
    return rc;
  }

  Inventory iv{};

  int res = sqlite3_step(stmt);
  if (res == SQLITE_ROW) {
    auto mapper = ResultSetMapper<Inventory>{};
    mapper.init_mapper(stmt);
    mapper.map_row(stmt, iv);
    fmt::println("{}", iv.name);
  }

  // ess::orm::config::print_config();

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
}
