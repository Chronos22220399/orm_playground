// SQLite3 vs ORM 文件数据库性能对比基准测试
// 使用文件数据库，测试不同数据规模：10, 100, 1000, 10000

#include <benchmark/benchmark.h>
#include <ess/orm/orm.hpp>
#include <filesystem>
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

namespace fs = std::filesystem;

// ==================== 数据库文件路径 ====================
// 从 build/bench/ 运行时，使用 ../../bench/data
constexpr const char *SQLITE_DB_PATH = "../../bench/data/sqlite_benchmark.db";
constexpr const char *ORM_DB_PATH = "../../bench/data/orm_benchmark.db";

// ==================== 测试数据库配置 ====================
struct SQLiteBenchDB {
  static constexpr std::string_view connection_url = SQLITE_DB_PATH;
  static constexpr std::size_t pool_size = 1;
};

struct ORMBenchDB {
  static constexpr std::string_view connection_url = ORM_DB_PATH;
  static constexpr std::size_t pool_size = 10;
};

// ==================== 测试表结构 ====================

struct ORMUser {
  long long id{};
  std::string name{};
  int age{0};
  double score{0.0};
  bool active{true};

  using Database = ORMBenchDB;
  using Schema =
      dsl::Schema<"benchmark_users",
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

// 确保数据目录存在
void ensure_data_dir() { fs::create_directories("../../bench/data"); }

// 清理旧的数据库文件
void cleanup_old_databases() {
  std::string sqlite_path(SQLITE_DB_PATH);
  std::string orm_path(ORM_DB_PATH);

  if (fs::exists(sqlite_path)) {
    fs::remove(sqlite_path);
  }
  if (fs::exists(orm_path)) {
    fs::remove(orm_path);
  }

  // 也删除相关的WAL文件
  std::vector<std::string> extra_files = {sqlite_path + "-shm",
                                          sqlite_path + "-wal",
                                          orm_path + "-shm", orm_path + "-wal"};

  for (const auto &file : extra_files) {
    if (fs::exists(file)) {
      fs::remove(file);
    }
  }
}

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

// ==================== SQLite3原生API基准测试（文件数据库）
// ====================

class FileSQLite3Benchmark {
  sqlite3 *db_ = nullptr;
  sqlite3_stmt *insert_stmt_ = nullptr;
  sqlite3_stmt *select_limit_stmt_ = nullptr;
  sqlite3_stmt *update_stmt_ = nullptr;
  sqlite3_stmt *delete_stmt_ = nullptr;

public:
  FileSQLite3Benchmark() {
    // 打开文件数据库
    int rc = sqlite3_open(SQLITE_DB_PATH, &db_);
    if (rc != SQLITE_OK) {
      throw std::runtime_error("无法打开SQLite3数据库: " +
                               std::string(sqlite3_errmsg(db_)));
    }

    // 开启性能优化
    sqlite3_exec(db_, "PRAGMA synchronous = NORMAL", nullptr, nullptr, nullptr);
    sqlite3_exec(db_, "PRAGMA journal_mode = WAL", nullptr, nullptr, nullptr);
    sqlite3_exec(db_, "PRAGMA cache_size = 10000", nullptr, nullptr, nullptr);

    // 创建表
    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS benchmark_users ("
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

    // 预编译语句
    const char *insert_sql = "INSERT INTO benchmark_users (name, age, score, "
                             "active) VALUES (?, ?, ?, ?)";
    sqlite3_prepare_v2(db_, insert_sql, -1, &insert_stmt_, nullptr);

    const char *select_limit_sql =
        "SELECT id, name, age, score, active FROM benchmark_users LIMIT ?";
    sqlite3_prepare_v2(db_, select_limit_sql, -1, &select_limit_stmt_, nullptr);

    const char *update_sql = "UPDATE benchmark_users SET name = ? WHERE id = ?";
    sqlite3_prepare_v2(db_, update_sql, -1, &update_stmt_, nullptr);

    const char *delete_sql = "DELETE FROM benchmark_users WHERE id = ?";
    sqlite3_prepare_v2(db_, delete_sql, -1, &delete_stmt_, nullptr);
  }

  ~FileSQLite3Benchmark() {
    if (insert_stmt_)
      sqlite3_finalize(insert_stmt_);
    if (select_limit_stmt_)
      sqlite3_finalize(select_limit_stmt_);
    if (update_stmt_)
      sqlite3_finalize(update_stmt_);
    if (delete_stmt_)
      sqlite3_finalize(delete_stmt_);
    if (db_)
      sqlite3_close(db_);
  }

  // 批量插入（使用事务）
  void insert_batch(const std::vector<SQLiteUser> &users) {
    sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    for (const auto &user : users) {
      sqlite3_reset(insert_stmt_);
      sqlite3_bind_text(insert_stmt_, 1, user.name.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_int(insert_stmt_, 2, user.age);
      sqlite3_bind_double(insert_stmt_, 3, user.score);
      sqlite3_bind_int(insert_stmt_, 4, user.active ? 1 : 0);
      sqlite3_step(insert_stmt_);
    }

    sqlite3_exec(db_, "COMMIT", nullptr, nullptr, nullptr);
  }

  // 查询多个用户
  std::vector<SQLiteUser> query_multiple(int limit) {
    sqlite3_reset(select_limit_stmt_);
    sqlite3_bind_int(select_limit_stmt_, 1, limit);

    std::vector<SQLiteUser> users;
    while (sqlite3_step(select_limit_stmt_) == SQLITE_ROW) {
      SQLiteUser user;
      user.id = sqlite3_column_int64(select_limit_stmt_, 0);
      user.name = reinterpret_cast<const char *>(
          sqlite3_column_text(select_limit_stmt_, 1));
      user.age = sqlite3_column_int(select_limit_stmt_, 2);
      user.score = sqlite3_column_double(select_limit_stmt_, 3);
      user.active = sqlite3_column_int(select_limit_stmt_, 4) != 0;
      users.push_back(user);
    }

    return users;
  }

  // 批量更新（更新前N条记录）
  void update_batch(int count, const std::string &new_name_prefix) {
    sqlite3_exec(db_, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    for (int i = 1; i <= count; ++i) {
      sqlite3_reset(update_stmt_);
      sqlite3_bind_text(update_stmt_, 1,
                        (new_name_prefix + std::to_string(i)).c_str(), -1,
                        SQLITE_STATIC);
      sqlite3_bind_int(update_stmt_, 2, i);
      sqlite3_step(update_stmt_);
    }

    sqlite3_exec(db_, "COMMIT", nullptr, nullptr, nullptr);
  }

  // 批量删除（删除前N条记录）- 使用单条 SQL
  void delete_batch(int count) {
    char sql[100];
    snprintf(sql, sizeof(sql), "DELETE FROM benchmark_users WHERE id <= %d",
             count);
    sqlite3_exec(db_, sql, nullptr, nullptr, nullptr);
  }

  // 清空表
  void clear_table() {
    sqlite3_exec(db_, "DELETE FROM benchmark_users", nullptr, nullptr, nullptr);
    sqlite3_exec(db_, "VACUUM", nullptr, nullptr, nullptr); // 清理空间
  }

  // 获取记录数量
  int get_count() {
    sqlite3_stmt *stmt = nullptr;
    const char *sql = "SELECT COUNT(*) FROM benchmark_users";
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
      count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
  }
};

// ==================== ORM基准测试（文件数据库） ====================

class FileORMBenchmark {
public:
  FileORMBenchmark() {
    auto &ctx = Context::instance();
    ctx.register_db<ORMBenchDB>();

    auto ddl = ORMUser::Schema::make_create_table_ddl();
    auto &pool = ctx.conn_pool<ORMBenchDB>();
    auto conn = pool.acquire();
    conn->execute_raw(ddl);
  }

  // 批量插入（使用事务）
  void insert_batch(const std::vector<SQLiteUser> &users) {
    transaction<core::Write, ORMBenchDB>([&](auto &tx) {
      for (const auto &user : users) {
        tx.template query<ORMUser, "INSERT INTO benchmark_users (name, age, "
                                   "score, active) VALUES (?, ?, ?, ?)">(
            user.name, user.age, user.score, user.active);
      }
    });
  }

  // 查询多个用户 - 使用编译时 LIMIT 和 ContainerSize
  template <size_t N> std::vector<ORMUser> query_multiple_orm() {
    return query<ORMUser, "SELECT * FROM benchmark_users LIMIT ?"_sql,
                 ORMBenchDB, std::vector, core::ContainerSize<N>>(
        static_cast<int>(N));
  }

  // 便捷函数包装
  std::vector<ORMUser> query_multiple_orm(int limit) {
    if (limit == 10)
      return query_multiple_orm<10>();
    if (limit == 100)
      return query_multiple_orm<100>();
    if (limit == 1000)
      return query_multiple_orm<1000>();
    if (limit == 10000)
      return query_multiple_orm<10000>();
    return query<ORMUser, "SELECT * FROM benchmark_users LIMIT ?"_sql,
                 ORMBenchDB, std::vector, core::ContainerSize<100>>(limit);
  }

  // 批量更新
  void update_batch(int count, const std::string &new_name_prefix) {
    transaction<core::Write, ORMBenchDB>([&](auto &tx) {
      for (int i = 1; i <= count; ++i) {
        tx.template query<
            ORMUser, "UPDATE benchmark_users SET name = ? WHERE id = ?"_sql>(
            new_name_prefix + std::to_string(i), i);
      }
    });
  }

  // 批量删除 - 使用单条 SQL 批量删除，更高效
  void delete_batch(int count) {
    auto &pool = Context::instance().conn_pool<ORMBenchDB>();
    auto conn = pool.acquire();
    std::string sql =
        "DELETE FROM benchmark_users WHERE id <= " + std::to_string(count);
    conn->execute_raw(sql);
  }

  // 清空表
  void clear_table() {
    auto &pool = Context::instance().conn_pool<ORMBenchDB>();
    auto conn = pool.acquire();
    conn->execute_raw("DELETE FROM benchmark_users");
    conn->execute_raw("VACUUM");
  }
};

// ==================== 基准测试函数 ====================

// 全局初始化函数（在基准测试开始前执行一次）
static void global_setup() {
  ensure_data_dir();
  cleanup_old_databases();

  // 为ORM创建空的数据库文件（因为ORM需要文件已存在）
  {
    sqlite3 *db = nullptr;
    if (sqlite3_open(ORM_DB_PATH, &db) == SQLITE_OK) {
      sqlite3_close(db);
    }
  }

  std::cout << "文件数据库基准测试初始化完成" << std::endl;
  std::cout << "SQLite3数据库: " << SQLITE_DB_PATH << std::endl;
  std::cout << "ORM数据库: " << ORM_DB_PATH << std::endl;
}

// SQLite3批量插入基准测试
static void BM_File_SQLite3_InsertBatch(benchmark::State &state) {
  static bool initialized = false;
  if (!initialized) {
    global_setup();
    initialized = true;
  }

  int batch_size = state.range(0);
  FileSQLite3Benchmark bench;
  auto test_data = generate_test_data(batch_size);

  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.clear_table(); // 每次迭代后清空，确保公平
  }

  state.SetComplexityN(batch_size);
}

// ORM批量插入基准测试
static void BM_File_ORM_InsertBatch(benchmark::State &state) {
  static bool initialized = false;
  if (!initialized) {
    global_setup();
    initialized = true;
  }

  int batch_size = state.range(0);
  FileORMBenchmark bench;
  auto test_data = generate_test_data(batch_size);

  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.clear_table();
  }

  state.SetComplexityN(batch_size);
}

// SQLite3批量查询基准测试
static void BM_File_SQLite3_QueryBatch(benchmark::State &state) {
  static bool initialized = false;
  static FileSQLite3Benchmark bench;
  static std::vector<SQLiteUser> test_data;

  if (!initialized) {
    global_setup();
    test_data = generate_test_data(10000);
    bench.insert_batch(test_data);
    initialized = true;
  }

  int query_size = state.range(0);

  for (auto _ : state) {
    benchmark::DoNotOptimize(bench.query_multiple(query_size));
  }

  state.SetComplexityN(query_size);
}

// ORM批量查询基准测试
static void BM_File_ORM_QueryBatch(benchmark::State &state) {
  static bool initialized = false;
  static FileORMBenchmark bench;
  static std::vector<SQLiteUser> test_data;

  if (!initialized) {
    global_setup();
    test_data = generate_test_data(10000);
    bench.insert_batch(test_data);
    initialized = true;
  }

  int query_size = state.range(0);

  for (auto _ : state) {
    benchmark::DoNotOptimize(bench.query_multiple_orm(query_size));
  }

  state.SetComplexityN(query_size);
}

// SQLite3批量更新基准测试
static void BM_File_SQLite3_UpdateBatch(benchmark::State &state) {
  static bool initialized = false;
  static FileSQLite3Benchmark bench;
  static std::vector<SQLiteUser> test_data;

  int batch_size = state.range(0);

  if (!initialized) {
    global_setup();
    test_data = generate_test_data(batch_size);
    initialized = true;
  }

  // 每次迭代前插入数据，然后更新
  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.update_batch(batch_size, "Updated_");
  }

  state.SetComplexityN(batch_size);
}

// ORM批量更新基准测试
static void BM_File_ORM_UpdateBatch(benchmark::State &state) {
  static bool initialized = false;
  static FileORMBenchmark bench;
  static std::vector<SQLiteUser> test_data;

  int batch_size = state.range(0);

  if (!initialized) {
    global_setup();
    test_data = generate_test_data(batch_size);
    initialized = true;
  }

  // 每次迭代前插入数据，然后更新
  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.update_batch(batch_size, "Updated_");
  }

  state.SetComplexityN(batch_size);
}

// SQLite3批量删除基准测试
static void BM_File_SQLite3_DeleteBatch(benchmark::State &state) {
  static bool initialized = false;
  static FileSQLite3Benchmark bench;
  static std::vector<SQLiteUser> test_data;

  int batch_size = state.range(0);

  if (!initialized) {
    global_setup();
    test_data = generate_test_data(batch_size);
    initialized = true;
  }

  // 每次迭代前插入数据，然后删除
  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.delete_batch(batch_size);
  }

  state.SetComplexityN(batch_size);
}

// ORM批量删除基准测试
static void BM_File_ORM_DeleteBatch(benchmark::State &state) {
  static bool initialized = false;
  static FileORMBenchmark bench;
  static std::vector<SQLiteUser> test_data;

  int batch_size = state.range(0);

  if (!initialized) {
    global_setup();
    test_data = generate_test_data(batch_size);
    initialized = true;
  }

  // 每次迭代前插入数据，然后删除
  for (auto _ : state) {
    bench.insert_batch(test_data);
    bench.delete_batch(batch_size);
  }

  state.SetComplexityN(batch_size);
}

// ==================== 注册基准测试 ====================

// 批量插入测试：10, 100, 1000, 10000
BENCHMARK(BM_File_SQLite3_InsertBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Complexity(benchmark::oN);

BENCHMARK(BM_File_ORM_InsertBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Complexity(benchmark::oN);

// 批量查询测试：10, 100, 1000, 10000
BENCHMARK(BM_File_SQLite3_QueryBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Complexity(benchmark::oN);

BENCHMARK(BM_File_ORM_QueryBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Arg(10000)
    ->Complexity(benchmark::oN);

// 批量更新测试：10, 100, 1000
BENCHMARK(BM_File_SQLite3_UpdateBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Complexity(benchmark::oN);

BENCHMARK(BM_File_ORM_UpdateBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Complexity(benchmark::oN);

// 批量删除测试：10, 100, 1000
BENCHMARK(BM_File_SQLite3_DeleteBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Complexity(benchmark::oN);

BENCHMARK(BM_File_ORM_DeleteBatch)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->Complexity(benchmark::oN);

// ==================== 主函数 ====================
int main(int argc, char **argv) {
  // 确保数据目录存在
  ensure_data_dir();

  ::benchmark::Initialize(&argc, argv);

  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;

  std::cout << "开始文件数据库性能基准测试..." << std::endl;
  std::cout << "测试数据规模: 10, 100, 1000, 10000" << std::endl;
  std::cout << "测试操作: 插入、查询、更新、删除" << std::endl;
  std::cout << "==========================================" << std::endl;

  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();

  // 清理测试文件
  cleanup_old_databases();

  return 0;
}
