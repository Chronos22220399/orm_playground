#pragma once
#include <optional>
#include <random>
#include <sqlite3.h>
#include <string>
#include <vector>

namespace bench {

struct RawGoods {
  int id;
  std::string title;
  double price;
  int stock;
  int status;
  int enabled;
};

class DataGenerator {
  std::mt19937 m_rng;

public:
  explicit DataGenerator(unsigned seed = 42) : m_rng(seed) {}

  std::vector<RawGoods> generate(std::size_t count) {
    std::vector<RawGoods> result;
    result.reserve(count);

    std::uniform_real_distribution<double> price_dist(1.0, 1000.0);
    std::uniform_int_distribution<int> stock_dist(0, 1000);
    std::uniform_int_distribution<int> status_dist(0, 2);
    std::uniform_int_distribution<int> bool_dist(0, 1);

    for (std::size_t i = 0; i < count; ++i) {
      result.push_back({static_cast<int>(i + 1), "Product_" + std::to_string(i),
                        price_dist(m_rng), stock_dist(m_rng),
                        status_dist(m_rng), bool_dist(m_rng)});
    }
    return result;
  }

  int random_id(int max_id) {
    std::uniform_int_distribution<int> dist(1, max_id);
    return dist(m_rng);
  }
};

class NativeSqlite {
  sqlite3 *m_db = nullptr;
  sqlite3_stmt *m_insert_stmt = nullptr;
  sqlite3_stmt *m_select_all_stmt = nullptr;
  sqlite3_stmt *m_select_one_stmt = nullptr;
  sqlite3_stmt *m_update_stmt = nullptr;
  sqlite3_stmt *m_count_stmt = nullptr;

public:
  explicit NativeSqlite(const char *path = "./bench/data/test_raw.db") {
    sqlite3_open(path, &m_db);

    exec("PRAGMA journal_mode=WAL");
    exec("PRAGMA synchronous=NORMAL");
    exec("PRAGMA cache_size=-64000");
    exec("PRAGMA temp_store=MEMORY");
    exec("PRAGMA busy_timeout=30000");

    exec(R"(
         DROP TABLE IF EXISTS goods
         )");

    exec(R"(
            CREATE TABLE IF NOT EXISTS goods (
                id INTEGER PRIMARY KEY,
                title TEXT NOT NULL,
                price REAL,
                stock INTEGER,
                status INTEGER,
                enabled INTEGER
            )
        )");

    prepare_statements();
  }

  ~NativeSqlite() {
    if (m_insert_stmt)
      sqlite3_finalize(m_insert_stmt);
    if (m_select_all_stmt)
      sqlite3_finalize(m_select_all_stmt);
    if (m_select_one_stmt)
      sqlite3_finalize(m_select_one_stmt);
    if (m_update_stmt)
      sqlite3_finalize(m_update_stmt);
    if (m_count_stmt)
      sqlite3_finalize(m_count_stmt);
    if (m_db)
      sqlite3_close(m_db);
  }

  NativeSqlite(const NativeSqlite &) = delete;
  NativeSqlite &operator=(const NativeSqlite &) = delete;

  void exec(const char *sql) {
    sqlite3_exec(m_db, sql, nullptr, nullptr, nullptr);
  }

  void clear() { exec("DELETE FROM goods"); }

  void insert(const RawGoods &g) {
    sqlite3_bind_int(m_insert_stmt, 1, g.id);
    sqlite3_bind_text(m_insert_stmt, 2, g.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(m_insert_stmt, 3, g.price);
    sqlite3_bind_int(m_insert_stmt, 4, g.stock);
    sqlite3_bind_int(m_insert_stmt, 5, g.status);
    sqlite3_bind_int(m_insert_stmt, 6, g.enabled);
    sqlite3_step(m_insert_stmt);
    sqlite3_reset(m_insert_stmt);
    sqlite3_clear_bindings(m_insert_stmt);
  }

  void insert_batch(const std::vector<RawGoods> &goods) {
    exec("BEGIN IMMEDIATE");
    for (const auto &g : goods)
      insert(g);
    exec("COMMIT");
  }

  std::vector<RawGoods> select_all() {
    std::vector<RawGoods> result;
    while (sqlite3_step(m_select_all_stmt) == SQLITE_ROW) {
      RawGoods g;
      g.id = sqlite3_column_int(m_select_all_stmt, 0);
      auto text = sqlite3_column_text(m_select_all_stmt, 1);
      g.title = text ? reinterpret_cast<const char *>(text) : "";
      g.price = sqlite3_column_double(m_select_all_stmt, 2);
      g.stock = sqlite3_column_int(m_select_all_stmt, 3);
      g.status = sqlite3_column_int(m_select_all_stmt, 4);
      g.enabled = sqlite3_column_int(m_select_all_stmt, 5);
      result.push_back(std::move(g));
    }
    sqlite3_reset(m_select_all_stmt);
    return result;
  }

  std::optional<RawGoods> select_one(int id) {
    sqlite3_bind_int(m_select_one_stmt, 1, id);
    std::optional<RawGoods> result;
    if (sqlite3_step(m_select_one_stmt) == SQLITE_ROW) {
      RawGoods g;
      g.id = sqlite3_column_int(m_select_one_stmt, 0);
      auto text = sqlite3_column_text(m_select_one_stmt, 1);
      g.title = text ? reinterpret_cast<const char *>(text) : "";
      g.price = sqlite3_column_double(m_select_one_stmt, 2);
      g.stock = sqlite3_column_int(m_select_one_stmt, 3);
      g.status = sqlite3_column_int(m_select_one_stmt, 4);
      g.enabled = sqlite3_column_int(m_select_one_stmt, 5);
      result = std::move(g);
    }
    sqlite3_reset(m_select_one_stmt);
    sqlite3_clear_bindings(m_select_one_stmt);
    return result;
  }

  void update(int id, const std::string &title) {
    sqlite3_bind_text(m_update_stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(m_update_stmt, 2, id);
    sqlite3_step(m_update_stmt);
    sqlite3_reset(m_update_stmt);
    sqlite3_clear_bindings(m_update_stmt);
  }

  int count() {
    sqlite3_step(m_count_stmt);
    int result = sqlite3_column_int(m_count_stmt, 0);
    sqlite3_reset(m_count_stmt);
    return result;
  }

  sqlite3 *handle() { return m_db; }

private:
  void prepare_statements() {
    sqlite3_prepare_v2(m_db, "INSERT INTO goods VALUES (?, ?, ?, ?, ?, ?)", -1,
                       &m_insert_stmt, nullptr);
    sqlite3_prepare_v2(m_db, "SELECT * FROM goods", -1, &m_select_all_stmt,
                       nullptr);
    sqlite3_prepare_v2(m_db, "SELECT * FROM goods WHERE id = ?", -1,
                       &m_select_one_stmt, nullptr);
    sqlite3_prepare_v2(m_db, "UPDATE goods SET title = ? WHERE id = ?", -1,
                       &m_update_stmt, nullptr);
    sqlite3_prepare_v2(m_db, "SELECT COUNT(*) FROM goods", -1, &m_count_stmt,
                       nullptr);
  }
};

} // namespace bench
