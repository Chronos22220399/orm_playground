// SQLite3 vs ORM 公平性能对比基准测试
// 确保两者使用相同的优化级别

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
  static constexpr std::size_t pool_size = 10; // 使用单个连接，更公平
};

// ==================== 测试表结构 ====================

// ORM使用的表结构
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

// SQLite3使用的简单结构
struct SQLiteUser {
  long long id{};
  std::string name{};
  int age{0};
  double score{0.0};
  bool active{true};
};

// ==================== 辅助函数 ====================

// 生成随机字符串
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

// 生成测试数据
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

// ==================== SQLite3原生API基准测试（优化版） ====================

class OptimizedSQLite3Benchmark {
  sqlite3 *db_ = nullptr;
  // 预编译语句
  sqlite3_stmt *insert_stmt_ = nullptr;
  sqlite3_stmt *select_stmt_ = nullptr;
  sqlite3_stmt *update_stmt_ = nullptr;
  sqlite3_stmt *delete_stmt_ = nullptr;

public:
  OptimizedSQLite3Benchmark() {
    // 打开内存数据库
    int rc = sqlite3_open("file::memory:?cache=shared", &db_);
    if (rc != SQLITE_OK) {
      throw std::runtime_error("无法打开SQLite3数据库");
    }

    // 开启SQLite3性能优化（与ORM公平对比）
    sqlite3_exec(db_, "PRAGMA synchronous = OFF", nullptr, nullptr, nullptr);
    sqlite3_exec(db_, "PRAGMA journal_mode = MEMORY", nullptr, nullptr,
                 nullptr);
    sqlite3_exec(db_, "PRAGMA cache_size = 10000", nullptr, nullptr, nullptr);

    // 创建表
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

    // 预编译所有语句（模拟ORM的编译时优化）
    const char *insert_sql = "INSERT INTO benchmark_sqlite_users (name, age, "
                             "score, active) VALUES (?, ?, ?, ?)";
    sqlite3_prepare_v2(db_, insert_sql, -1, &insert_stmt_, nullptr);

    const char *select_sql = "SELECT id, name, age, score, active FROM "
                             "benchmark_sqlite_users WHERE id = ?";
    sqlite3_prepare_v2(db_, select_sql, -1, &select_stmt_, nullptr);

    const char *update_sql =
        "UPDATE benchmark_sqlite_users SET name = ? WHERE id = ?";
    sqlite3_prepare_v2(db_, update_sql, -1, &update_stmt_, nullptr);

    const char *delete_sql = "DELETE FROM benchmark_sqlite_users WHERE id = ?";
    sqlite3_prepare_v2(db_, delete_sql, -1, &delete_stmt_, nullptr);
  }

  ~OptimizedSQLite3Benchmark() {
    if (insert_stmt_)
      sqlite3_finalize(insert_stmt_);
    if (select_stmt_)
      sqlite3_finalize(select_stmt_);
    if (update_stmt_)
      sqlite3_finalize(update_stmt_);
    if (delete_stmt_)
      sqlite3_finalize(delete_stmt_);
    if (db_)
      sqlite3_close(db_);
  }

  // 插入单个用户
  void insert_single(const SQLiteUser &user) {
    sqlite3_reset(insert_stmt_);
    sqlite3_bind_text(insert_stmt_, 1, user.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(insert_stmt_, 2, user.age);
    sqlite3_bind_double(insert_stmt_, 3, user.score);
    sqlite3_bind_int(insert_stmt_, 4, user.active ? 1 : 0);
    sqlite3_step(insert_stmt_);
  }

  // 查询单个用户
  SQLiteUser query_single(int id) {
    sqlite3_reset(select_stmt_);
    sqlite3_bind_int(select_stmt_, 1, id);

    SQLiteUser user;
    if (sqlite3_step(select_stmt_) == SQLITE_ROW) {
      user.id = sqlite3_column_int64(select_stmt_, 0);
      user.name =
          reinterpret_cast<const char *>(sqlite3_column_text(select_stmt_, 1));
      user.age = sqlite3_column_int(select_stmt_, 2);
      user.score = sqlite3_column_double(select_stmt_, 3);
      user.active = sqlite3_column_int(select_stmt_, 4) != 0;
    }

    return user;
  }

  // 更新用户
  void update_single(int id, const std::string &new_name) {
    sqlite3_reset(update_stmt_);
    sqlite3_bind_text(update_stmt_, 1, new_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(update_stmt_, 2, id);
    sqlite3_step(update_stmt_);
  }

  // 删除用户
  void delete_single(int id) {
    sqlite3_reset(delete_stmt_);
    sqlite3_bind_int(delete_stmt_, 1, id);
    sqlite3_step(delete_stmt_);
  }

  // 清空表
  void clear_table() {
    sqlite3_exec(db_, "DELETE FROM benchmark_sqlite_users", nullptr, nullptr,
                 nullptr);
  }
};

// ==================== ORM基准测试 ====================

class ORMBenchmark {
public:
  ORMBenchmark() {
    // 注册数据库
    auto &ctx = Context::instance();
    ctx.register_db<BenchDB>();

    // 创建表
    auto ddl = ORMUser::Schema::make_create_table_ddl();
    auto &pool = ctx.conn_pool<BenchDB>();
    auto conn = pool.acquire();
    conn->execute_raw(ddl);
  }

  // 插入单个用户
  void insert_single(const SQLiteUser &user) {
    // 注意：这里使用SQLiteUser，需要转换为ORMUser
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

  // 查询单个用户
  ORMUser query_single_orm(int id) {
    auto results =
        query<ORMUser, "SELECT * FROM benchmark_orm_users WHERE id = ?"_sql,
              BenchDB>(id);

    if (results.empty())
      return ORMUser{};

    return results[0];
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
  OptimizedSQLite3Benchmark bench;
  auto test_data = generate_test_data(100);

  // 先插入一些数据，避免空表的影响
  for (int i = 0; i < 10; ++i) {
    bench.insert_single(test_data[i]);
  }

  for (auto _ : state) {
    bench.insert_single(test_data[state.iterations() % 100]);
  }

  bench.clear_table();
}
BENCHMARK(BM_SQLite3_InsertSingle);

// ORM插入单个基准测试
static void BM_ORM_InsertSingle(benchmark::State &state) {
  ORMBenchmark bench;
  auto test_data = generate_test_data(100);
  bench.insert_batch(test_data);

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 100);

  for (auto _ : state) {
    int id = id_dist(rng);
    benchmark::DoNotOptimize(bench.query_single_orm(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_ORM_InsertSingle);

// SQLite3查询单个基准测试
static void BM_SQLite3_QuerySingle(benchmark::State &state) {
  OptimizedSQLite3Benchmark bench;
  auto test_data = generate_test_data(100);

  // 插入测试数据
  for (const auto &user : test_data) {
    bench.insert_single(user);
  }

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 100);

  for (auto _ : state) {
    int id = id_dist(rng);
    benchmark::DoNotOptimize(bench.query_single(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_SQLite3_QuerySingle);

// ORM查询单个基准测试
static void BM_ORM_QuerySingle(benchmark::State &state) {
  ORMBenchmark bench;
  auto test_data = generate_test_data(100);

  // 插入测试数据
  for (const auto &user : test_data) {
    bench.insert_single(user);
  }

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 100);

  for (auto _ : state) {
    int id = id_dist(rng);
    benchmark::DoNotOptimize(bench.query_single(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_ORM_QuerySingle);

// SQLite3更新基准测试
static void BM_SQLite3_Update(benchmark::State &state) {
  OptimizedSQLite3Benchmark bench;
  auto test_data = generate_test_data(100);

  // 插入测试数据
  for (const auto &user : test_data) {
    bench.insert_single(user);
  }

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 100);

  for (auto _ : state) {
    int id = id_dist(rng);
    bench.update_single(id, "UpdatedName" + std::to_string(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_SQLite3_Update);

// ORM更新基准测试
static void BM_ORM_Update(benchmark::State &state) {
  ORMBenchmark bench;
  auto test_data = generate_test_data(100);

  // 插入测试数据
  for (const auto &user : test_data) {
    bench.insert_single(user);
  }

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> id_dist(1, 100);

  for (auto _ : state) {
    int id = id_dist(rng);
    bench.update_single(id, "UpdatedName" + std::to_string(id));
  }

  bench.clear_table();
}
BENCHMARK(BM_ORM_Update);

// SQLite3删除基准测试
static void BM_SQLite3_Delete(benchmark::State &state) {
  OptimizedSQLite3Benchmark bench;

  // 使用静态计数器避免重复删除
  static int current_id = 1;

  for (auto _ : state) {
    if (current_id > 100) {
      // 重置数据
      state.PauseTiming();
      auto test_data = generate_test_data(100);
      for (const auto &user : test_data) {
        bench.insert_single(user);
      }
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
  ORMBenchmark bench;

  // 使用静态计数器避免重复删除
  static int current_id = 1;

  for (auto _ : state) {
    if (current_id > 100) {
      // 重置数据
      state.PauseTiming();
      auto test_data = generate_test_data(100);
      for (const auto &user : test_data) {
        bench.insert_single(user);
      }
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
