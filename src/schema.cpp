#include <core.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/connection_pool.h>
#include <ess/orm/context.hpp>
#include <ess/orm/parser.hpp>
#include <ess/orm/result_set_mapper.hpp>
#include <ess/orm/runtime.hpp>
#include <ess/orm/statement.h>
#include <ess/orm/transaction.hpp>
#include <thread>
// #include <ess/orm/test/stress_test.hpp>
#include <sqlite3.h>

using namespace ess::orm;
using namespace ess::orm::meta;
using namespace ess::orm::dsl;
using namespace ess::orm::attribute;

enum class GoodsStatus : int { Normal = 0, Disabled, Deleted };

struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;

  using Schema = Schema<
      "goods", //
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<GoodsStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>> //
      >;
};

template <size_t N> void println(const ess::orm::meta::FixedString<N> &str) {
  fmt::println("{}", std::string_view(str));
}

struct Foo {};

void test_row();

void test_multithread();

void init_database();

void test_row_asan_safety();

int main() {
  using t = attribute::DefaultValue<10.0_fp>;
  // static_assert(concepts::floating_point_wrapper<decltype(10.0_fp)>);
  // Goods goods{};
  // Goods::Schema::make_create_table_ddl();
  // init_database();
  // test_row();
  static_assert(parser::begin_with<meta::fs_to_upper("select"_fs)>() ==
                parser::SqlType::SELECT);

  Context::instance().init();
  auto &pool = Context::instance().conn_pool();
  int level = 0;
  Transaction tx(pool.acquire(), level);
  tx.begin();
  std::vector<Goods> res = tx.query<Goods, "SELECT * FROM goods">();
  tx.commit();
  std::cout << res.size() << std::endl;

  // Row row;
  // try {
  //   auto v = row["missing"];
  // } catch (const std::exception &e) {
  //   std::cout << "caught: " << e.what() << '\n';
  // }

  // test_multithread();
  //
  // test_row_asan_safety();

  return 0;
}

// class Context {
//   inline static std::unique_ptr<ConnectionPool> m_conn_pool = nullptr;
//   inline static std::once_flag m_init_flag{};
//
// public:
//   static Context &instance() {
//     static Context ctx;
//     std::call_once(m_init_flag, []() { init(); });
//     return ctx;
//   }
//
//   static void init() {
//     m_conn_pool = std::make_unique<ConnectionPool>(config::connection_url,
//                                                    config::pool_size);
//   }
//
//   ConnectionPool &conn_pool() { return *m_conn_pool; }
// };

void test_row() {
  ConnectionPool pool =
      ConnectionPool(config::connection_url, config::pool_size);
  auto conn = pool.acquire();

  auto ddl = Goods::Schema::make_create_table_ddl();

  auto &stmt = conn->prepare_cached("SELECT * FROM goods WHERE title = ?");
  stmt.bind_params(std::string("hello"));

  Goods goods{};

  while (stmt.next()) {
    auto mapper = ResultSetMapper<Goods>{};
    mapper.init_mapper(stmt.get());
    auto row = mapper.map_row(stmt.get());

    int id = row.get_if<int>("id").value();
    auto title = row.get_if<std::string>("title").value();
    auto price = row.get_if<double>("price").value();
    auto stock = row.get_if<float>("stock").value();
    auto status = row.get_if<int>("status").value();
    auto enabled = row.get_if<bool>("enabled").value();

    // int id = row["id"];
    // auto title = row["title"].as<std::string>();
    // auto price = row["price"].as<double>();
    // auto stock = row["stock"].as<float>();
    // auto status = row["status"].as<int>();
    // auto enabled = row["enabled"].as<bool>();
    fmt::println("{} {} {} {} {} {}", id, title, price, stock, status, enabled);
    mapper.map_row(stmt.get(), goods);
    fmt::println("{} {} {} {} {} {}", goods.id, goods.title, goods.price,
                 goods.stock, (int)goods.status, goods.enabled);
  }

  auto conn_1 = pool.acquire();

  auto &stmt_1 = conn_1->prepare_cached("SELECT * FROM goods WHERE title = ?");
  stmt_1.bind_params(std::string("while"));

  while (stmt_1.next()) {
    auto mapper = ResultSetMapper<Goods>{};
    mapper.init_mapper(stmt_1.get());
    auto row = mapper.map_row(stmt_1.get());

    int id = row.get_if<int>("id").value();
    auto title = row.get_if<std::string>("title").value();
    auto price = row.get_if<double>("price").value();
    auto stock = row.get_if<float>("stock").value();
    auto status = row.get_if<int>("status").value();
    auto enabled = row.get_if<bool>("enabled").value();
    // auto price = row.get_if<double>("price").value();

    // int id = row["id"];
    // auto title = row["title"].as<std::string>();
    // auto price = row["price"].as<double>();
    // auto stock = row["stock"].as<float>();
    // auto status = row["status"].as<int>();
    // auto enabled = row["enabled"].as<bool>();
    fmt::println("{} {} {} {} {} {}", id, title, price, stock, status, enabled);
    mapper.map_row(stmt_1.get(), goods);
    fmt::println("{} {} {} {} {} {}", goods.id, goods.title, goods.price,
                 goods.stock, (int)goods.status, goods.enabled);
  }

  // ess::orm::config::print_config();
}

void test_multithread() {
  constexpr int NUM_THREADS = 100; // 减少线程数，更合理

  std::vector<std::thread> threads;
  std::atomic<int> success{0};
  std::atomic<int> failed{0};

  auto &conn_pool = Context::instance().conn_pool();

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([&conn_pool, i, &success, &failed]() {
      try {
        auto conn = conn_pool.acquire();
        auto &stmt = conn->prepare_cached("INSERT INTO logs (msgs) VALUES (?)");
        stmt.bind_params(fmt::format("Thread {}", i));
        stmt.next();
        stmt.reset();
        stmt.clear_bindings();
        ++success;
      } catch (const std::exception &e) {
        fmt::println(stderr, "Thread {} failed: {}", i, e.what());
        ++failed;
      }
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  fmt::println("成功: {}, 失败: {}", success.load(), failed.load());
}

void init_database() {
  auto conn = Context::instance().conn_pool().acquire();
  conn->execute_raw(R"(
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            msgs TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");
}

void test_row_asan_safety() {
  std::cerr << "\n--- 开始 Row 类型安全与 ASan 测试 ---\n";
  std::cerr.flush();

  using namespace ess::orm;
  Row row;
  row.add_column("id", 100LL);
  row.add_column("title", std::string("C++ ORM"));

  // 场景 3：访问不存在的列
  std::cerr << " - 场景 3: 开始\n";
  std::cerr.flush();

  std::cerr << "\n[MAIN] About to enter try block\n";
  std::cerr.flush();

  try {
    std::cerr << " - 场景 3: 即将调用 row[\"missing_column\"]\n";
    std::cerr.flush();

    std::cerr << "[MAIN] About to call operator[]\n";
    std::cerr.flush();

    row["missing_column"];

    std::cerr << "不应该到这里\n";
  } catch (const std::out_of_range &e) {
    std::cerr << "[MAIN] Caught std::out_of_range!\n";
    std::cerr << "✓ 捕��到 std::out_of_range:  " << e.what() << "\n";
    std::cerr.flush();
  } catch (const std::exception &e) {
    std::cerr << "[MAIN] Caught std:: exception!\n";
    std::cerr << "✓ 捕获到其他异常: " << e.what() << "\n";
    std::cerr.flush();
  } catch (...) {
    std::cerr << "[MAIN] Caught unknown exception!\n";
    std::cerr.flush();
    std::cerr << "✓ 捕获到未知异常\n";
  }

  std::cerr << "[MAIN] Exiting try-catch\n";
  std::cerr.flush();

  std::cerr << " - 场景 3 完成\n";
  std::cerr.flush();
}
