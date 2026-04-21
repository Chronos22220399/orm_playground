// SQLite3 vs ORM 性能对比基准测试（修复版）
// 基于公平基准测试的稳定版本

#include <benchmark/benchmark.h>
#include <ess/orm/orm.hpp>
#include <iostream>
#include <random>
#include <sqlite3.h>
#include <string>
#include <vector>

using namespace ess::orm;
using namespace ess::orm::dsl;
using namespace ess::orm::attribute;
using namespace ess::orm::meta;
using namespace ess::orm::sql;

// ==================== 测试数据库配置 ====================
struct BenchDB {
  static constexpr std::string_view connection_url =
      "file::memory:?cache=shared";
  static constexpr std::size_t pool_size = 1;
};

// ==================== 测试表结构 ====================

struct ORMUser {
  long long id{};
  std::string name{};
  int age{0};
  double score{0.0};
  bool active{true};

  using Database = BenchDB;
  using Schema =
      dsl::Schema<"benchmark_orm_users",
                  Field<"id", &ORMUser::id, PrimaryKey, AutoIncrement>,
                  Field<"name", &ORMUser::name>, Field<"age", &ORMUser::age>,
                  Field<"score", &ORMUser::score>,
                  Field<"active", &ORMUser::active, DefaultValue<true>>>;
};

struct SQLiteUser {
  long long id{};
  std::string name{};
  int age{0};
  double score{0.0};
  bool active{true};
};

// ==================== 辅助函数 ====================

std::string random_string(size_t length) {
  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  static std::mt19937 rng(std::random_device{}());
  static std::uniform_int_distribution<> dist(0, sizeof(alphanum) - 2);

  std::string str(length, '\0');
  for (size_t i = 0; i < length; ++i) {
    str[i] = alphanum[dist(rng)];
  }
  return str;
}

std::vector<SQLiteUser> generate_test_data(int count) {
  std::vector<SQLiteUser> users;
  users.reserve(count);

  static std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> age_dist(18, 80);
  std::uniform_real_distribution<double> score_dist(0.0, 100.0);
  std::uniform_int_distribution<int> name_len_dist(5, 15);

  for (int i = 0; i < count; ++i) {
    users.push_back(SQLiteUser{.id = 0,
                               .name = random_string(name_len_dist(rng)),
                               .age = age_dist(rng),
                               .score = score_dist(rng),
                               .active = (i % 10 != 0)});
  }

  return users;
}

// ==================== SQLite3原生API基准测试 ====================

class SimpleSQLite3Benchmark {
  sqlite3 *db_ = nullptr;

public:
  SimpleSQLite3Benchmark() {
    int rc = sqlite3_open("file::memory:?cache=shared", &db_);
    if (rc != SQLITE_OK) {
      throw std::runtime_error("无法打开SQLite3数据库");
    }

    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS benchmark_sqlite_users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "age INTEGER NOT NULL, "
        "score REAL NOT NULL, "
        "active BOOLEAN NOT NULL DEFAULT true)";

    char *err_msg = nullptr;
    rc = sqlite3_exec(db_, create_table_sql, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
      std::string error = err_msg ? err_msg : "未知错误";
      sqlite3_free(err_msg);
      throw std::runtime_error("创建表失败: " + error);
    }
  }

  ~SimpleSQLite3Benchmark() {
    if (db_) {
      sqlite3_close(db_);
    }
  }

  // 插入单个用户
  void insert_single(const SQLiteUser &user) {
    sqlite3_stmt *stmt = nullptr;
    const char *sql =
        "INSERT INTO benchmark_sqlite_users (name, age, score, active) "
        "VALUES (?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
      return;

    sqlite3_bind_text(stmt, 1, user.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user.age);
    sqlite3_bind_double(stmt, 3, user.score);
    sqlite3_bind_int(stmt, 4, user.active ? 1 : 0);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  // 批量插入
  void insert_batch(const std::vector<SQLiteUser> &users) {
    sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    sqlite3_stmt *stmt = nullptr;
    const char *sql =
        "INSERT INTO benchmark_sqlite_users (name, age, score, active) "
        "VALUES (?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
      return;

    for (const auto &user : users) {
      sqlite3_bind_text(stmt, 1, user.name.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_int(stmt, 2, user.age);
      sqlite3_bind_double(stmt, 3, user.score);
      sqlite3_bind_int(stmt, 4, user.active ? 1 : 0);

      sqlite3_step(stmt);
      sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "COMMIT", nullptr, nullptr, nullptr);
  }

  // 查询单个用户
  SQLiteUser query_single(int id) {
    sqlite3_stmt *stmt = nullptr;
    const char *sql = "SELECT id, name, age, score, active FROM "
                      "benchmark_sqlite_users WHERE id = ?";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
      return SQLiteUser{};

    sqlite3_bind_int(stmt, 1, id);

    SQLiteUser user;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
      user.id = sqlite3_column_int64(stmt, 0);
      user.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      user.age = sqlite3_column_int(stmt, 2);
      user.score = sqlite3_column_double(stmt, 3);
      user.active = sqlite3_column_int(stmt, 4) != 0;
    }

    sqlite3_finalize(stmt);
    return user;
  }

  // 查询多个用户
  std::vector<SQLiteUser> query_multiple(int limit) {
    sqlite3_stmt *stmt = nullptr;
    const char *sql = "SELECT id, name, age, score, active FROM "
                      "benchmark_sqlite_users LIMIT ?";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
      return {};

    sqlite3_bind_int(stmt, 1, limit);

    std::vector<SQLiteUser> users;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      SQLiteUser user;
      user.id = sqlite3_column_int64(stmt, 0);
      user.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
      user.age = sqlite3_column_int(stmt, 2);
      user.score = sqlite3_column_double(stmt, 3);
      user.active = sqlite3_column_int(stmt, 4) != 0;
      users.push_back(user);
    }

    sqlite3_finalize(stmt);
    return users;
  }

  // 更新用户
  void update_single(int id, const std::string &new_name) {
    sqlite3_stmt *stmt = nullptr;
    const char *sql = "UPDATE benchmark_sqlite_users SET name = ? WHERE id = ?";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
      return;

    sqlite3_bind_text(stmt, 1, new_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  // 删除用户
  void delete_single(int id) {
    sqlite3_stmt *stmt = nullptr;
    const char *sql = "DELETE FROM benchmark_sqlite_users WHERE id = ?";

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
      return;

    sqlite3_bind_int(stmt, 1, id);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  // 清空表
  void clear_table() {
    sqlite3_exec(db_, "DELETE FROM benchmark_sqlite_users", nullptr, nullptr,
                 nullptr);
  }
};

// ==================== ORM基准测试 ====================

class SimpleORMBenchmark {
public:
  SimpleORMBenchmark() {
    auto &ctx = Context::instance();
    ctx.register_db<BenchDB>();

    auto ddl = ORMUser::Schema::make_create_table_ddl();
    auto &pool = ctx.conn_pool<BenchDB>();
    auto conn = pool.acquire();
    conn->execute_raw(ddl);
  }

  // 插入单个用户
  void insert_single(const SQLiteUser &user) {
    ORMUser orm_user;
    orm_user.name = user.name;
    orm_user.age = user.age;
    orm_user.score = user.score;
    orm_user.active = user.active;

    query<ORMUser,
          "INSERT INTO benchmark_orm_users (name, age, score, active) VALUES "
          "(?, ?, ?, ?)",
          BenchDB>(orm_user.name, orm_user.age, orm_user.score,
                   orm_user.active);
  }

  // 批量插入
  void insert_batch(const std::vector<SQLiteUser> &users) {
    transaction<core::Write, BenchDB>([&](auto &tx) {
      for (const auto &user : users) {
        tx.template query<ORMUser, "INSERT INTO benchmark_orm_users (name, "
                                   "age, score, active) VALUES (?, ?, ?, ?)">(
            user.name, user.age, user.score, user.active);
      }
    });
  }

  // 查询单个用户
  ORMUser query_single_orm(int id) {
    auto results =
        query<ORMUser, "SELECT * FROM benchmark_orm_users WHERE id = ?"_sql,
              BenchDB>(id);

    if (results.empty())
      return {};

    return results[0];
  }

  // 查询多个用户
  std::vector<ORMUser> query_multiple_orm(int limit) {
    return query<ORMUser, "SELECT * FROM benchmark_orm_users LIMIT ?"_sql,
                 BenchDB>(limit);
  }

  // 查询多个用户
  std::vector<SQLiteUser> query_multiple(int limit) {
    std::vector<ORMUser> orm_results =
        query<ORMUser, "SELECT * FROM benchmark_orm_users LIMIT ?"_sql,
              BenchDB>(limit);
    // return orm_results;

    std::vector<SQLiteUser> users;
    users.reserve(orm_results.size());
    for (const auto &orm_user : orm_results) {
      users.push_back(SQLiteUser{.id = orm_user.id,
                                 .name = orm_user.name,
                                 .age = orm_user.age,
                                 .score = orm_user.score,
                                 .active = orm_user.active});
    }

    return users;
  }

  // 更新用户
  void update_single(int id, const std::string &new_name) {
    query<ORMUser, "UPDATE benchmark_orm_users SET name = ? WHERE id = ?"_sql,
          BenchDB>(new_name, id);
  }

  // 删除用户
  void delete_single(int id) {
    query<ORMUser, "DELETE FROM benchmark_orm_users WHERE id = ?"_sql, BenchDB>(
        id);
  }

  // 清空表
  void clear_table() {
    auto &pool = Context::instance().conn_pool<BenchDB>();
    auto conn = pool.acquire();
    conn->execute_raw("DELETE FROM benchmark_orm_users");
  }
};

// ==================== 基准测试函数 ====================

// SQLite3插入单个基准测试
static void BM_SQLite3_InsertSingle(benchmark::State &state) {
  SimpleSQLite3Benchmark bench;
  auto test_data = generate_test_data(1);

  for (auto _ : state) {
    bench.insert_single(test_data[0]);
  }

  bench.clear_table();
}
BENCHMARK(BM_SQLite3_InsertSingle);

// ORM插入单个基准测试
static void BM_ORM_InsertSingle(benchmark::State &state) {
  SimpleORMBenchmark bench;
  auto test_data = generate_test_data(1);

  for (auto _ : state) {
    bench.insert_single(test_data[0]);
  }

  bench.clear_table();
}
BENCHMARK(BM_ORM_InsertSingle);

// SQLite3批量插入基准测试
static void BM_SQLite3_InsertBatch(benchmark::State &state) {
  int batch_size = state.range(0);
  SimpleSQLite3Benchmark bench;
  auto test_data = generate_test_data(batch_size);

  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.clear_table();
  }

  state.SetComplexityN(batch_size);
}
BENCHMARK(BM_SQLite3_InsertBatch)->Arg(10)->Arg(100)->Arg(1000)->Complexity();

// ORM批量插入基准测试
static void BM_ORM_InsertBatch(benchmark::State &state) {
  int batch_size = state.range(0);
  SimpleORMBenchmark bench;
  auto test_data = generate_test_data(batch_size);

  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.clear_table();
  }

  state.SetComplexityN(batch_size);
}
BENCHMARK(BM_ORM_InsertBatch)->Arg(10)->Arg(100)->Arg(1000)->Complexity();

// SQLite3查询单个基准测试
static void BM_SQLite3_QuerySingle(benchmark::State &state) {
  SimpleSQLite3Benchmark bench;
  auto test_data = generate_test_data(1000);
  bench.insert_batch(test_data);

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 1000);

  for (auto _ : state) {
    int id = id_dist(rng);
    benchmark::DoNotOptimize(bench.query_single(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_SQLite3_QuerySingle);

// ORM查询单个基准测试
static void BM_ORM_QuerySingle(benchmark::State &state) {
  SimpleORMBenchmark bench;
  auto test_data = generate_test_data(1000);
  bench.insert_batch(test_data);

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 1000);

  for (auto _ : state) {
    int id = id_dist(rng);
    benchmark::DoNotOptimize(bench.query_single_orm(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_ORM_QuerySingle);

// SQLite3查询多个基准测试
static void BM_SQLite3_QueryMultiple(benchmark::State &state) {
  int limit = state.range(0);
  SimpleSQLite3Benchmark bench;
  auto test_data = generate_test_data(10000);
  bench.insert_batch(test_data);

  for (auto _ : state) {
    benchmark::DoNotOptimize(bench.query_multiple(limit));
  }

  bench.clear_table();
  state.SetComplexityN(limit);
}
BENCHMARK(BM_SQLite3_QueryMultiple)
    ->Arg(1)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Complexity();

// ORM查询多个基准测试
static void BM_ORM_QueryMultiple(benchmark::State &state) {
  int limit = state.range(0);
  SimpleORMBenchmark bench;
  auto test_data = generate_test_data(10000);
  bench.insert_batch(test_data);

  for (auto _ : state) {
    benchmark::DoNotOptimize(bench.query_multiple_orm(limit));
  }

  bench.clear_table();
  state.SetComplexityN(limit);
}
BENCHMARK(BM_ORM_QueryMultiple)
    ->Arg(1)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Complexity();

// SQLite3更新基准测试
static void BM_SQLite3_Update(benchmark::State &state) {
  SimpleSQLite3Benchmark bench;
  auto test_data = generate_test_data(1000);
  bench.insert_batch(test_data);

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 1000);

  for (auto _ : state) {
    int id = id_dist(rng);
    bench.update_single(id, "UpdatedName" + std::to_string(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_SQLite3_Update);

// ORM更新基准测试
static void BM_ORM_Update(benchmark::State &state) {
  SimpleORMBenchmark bench;
  auto test_data = generate_test_data(1000);
  bench.insert_batch(test_data);

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 1000);

  for (auto _ : state) {
    int id = id_dist(rng);
    bench.update_single(id, "UpdatedName" + std::to_string(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_ORM_Update);

// SQLite3删除基准测试
static void BM_SQLite3_Delete(benchmark::State &state) {
  SimpleSQLite3Benchmark bench;

  static int current_id = 1;

  for (auto _ : state) {
    if (current_id > 1000) {
      state.PauseTiming();
      auto test_data = generate_test_data(1000);
      bench.insert_batch(test_data);
      current_id = 1;
      state.ResumeTiming();
    }

    bench.delete_single(current_id);
    current_id++;
  }

  bench.clear_table();
}
BENCHMARK(BM_SQLite3_Delete);

// ORM删除基准测试
static void BM_ORM_Delete(benchmark::State &state) {
  SimpleORMBenchmark bench;

  static int current_id = 1;

  for (auto _ : state) {
    if (current_id > 1000) {
      state.PauseTiming();
      auto test_data = generate_test_data(1000);
      bench.insert_batch(test_data);
      current_id = 1;
      state.ResumeTiming();
    }

    bench.delete_single(current_id);
    current_id++;
  }

  bench.clear_table();
}
BENCHMARK(BM_ORM_Delete);

// ==================== 主函数 ====================
int main(int argc, char **argv) {
  ::benchmark::Initialize(&argc, argv);

  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;

  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();

  return 0;
}
