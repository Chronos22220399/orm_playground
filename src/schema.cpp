#include <core.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/connection_pool.h>
#include <ess/orm/context.hpp>
#include <ess/orm/parser.hpp>
#include <ess/orm/result_set_mapper.hpp>
#include <ess/orm/runtime.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/statement.h>
#include <ess/orm/transaction.hpp>
#include <print>
#include <ranges>
#include <thread>
// #include <ess/orm/test/stress_test.hpp>
#include <sqlite3.h>

using namespace ess::orm;
using namespace ess::orm::meta;
using namespace ess::orm::dsl;
using namespace ess::orm::attribute;

enum class GoodsStatus : int { Normal = 0, Disabled, Deleted };

void test_row();
void test_multithread();
void init_database();
void test_row_asan_safety();
void test_lexer_tokenize();

struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;

  using Database = config::default_db;
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

struct Log {
  int id;

  using Database = config::LoggerDB;
  using Schema = Schema<"log", Field<"id", &Log::id>>;
};

constexpr sql::Token ensure_no_error(sql::Token tok) {
  if (tok.type == sql::TokenType::UNKNOWN) {
    // 在编译期，这会导致编译器打印出具体的错误描述
    throw "SQL Lexer Error: Invalid character or unterminated literal!";
  }
  return tok;
}

template <meta::FixedString Sql> constexpr auto test_lexer_string() {
  auto lexer = sql::Lexer(Sql);
  auto t1 = lexer.next_token();
  auto t2 = lexer.next_token();
  auto t3 = lexer.next_token();
  return std::array{t1.type, t2.type, t3.type};
}

template <std::size_t Col> inline constexpr auto make_arrow(const char *src) {
  meta::FixedString<Col + 1> res{};
  for (std::size_t i = 0; i < Col; ++i) {
    res[i] = (src[i] == '\t') ? '\t' : ' ';
  }
  res[Col] = '^';
  return res;
}

constexpr std::string_view get_context_snippet(std::string_view s,
                                               std::size_t err_pos,
                                               std::size_t context_len = 40) {
  std::size_t start =
      (err_pos > context_len / 2) ? (err_pos - context_len / 2) : 0;

  std::size_t end =
      (start + context_len < s.size()) ? (start + context_len) : s.size();
  return s.substr(start, end - start);
}

int main() {
  using namespace ess::orm::config;

  // std::vector<int> vec = {1, 3, 4, 5};
  //
  // auto result = vec;
  // for (auto &r : result) {
  //   std::cout << r << std::endl;
  // }

  // constexpr auto sql =
  //     ", . * ( ) + - / ? = < <= <> > >= != ! hello SELECT INSERT hello?'"_fs;

  constexpr auto sql = "SELECT INSERT !"_fs;
  constexpr auto result = sql::Lexer(sql).tokenize<128>();
  // 运行时版本（更简单可靠）
  if constexpr (result.has_error) {
    constexpr std::size_t col = result.err_col;
    constexpr std::size_t pos = result.err_pos;
    constexpr std::size_t len = result.err_len;

    // 计算上下文范围
    constexpr std::size_t context_len = 50;
    constexpr std::size_t start =
        (pos > context_len / 2) ? (pos - context_len / 2) : 0;
    constexpr std::size_t end =
        ((start + context_len < sql.size()) ? (start + context_len)
                                            : sql.size()) -
        1;

    // 箭头位置 = 错误列 - 片段起始位置
    constexpr std::size_t arrow_pos = pos - start;

    fmt::println("[Ess-Orm] Error: {}", result.err_msg);
    fmt::println("Line {}, Column {}", result.err_line, col);
    fmt::println("  {}", meta::fs_substr<start, end - start>(sql));
    fmt::println("  {: >{}}^", "", arrow_pos);
    fmt::println("  Token: '{}'", fs_substr<pos, len>(sql));
  }

  auto conn = Context::instance().conn_pool().acquire();
  int level = 0;
  auto tx = Transaction(std::move(conn), level);
  try {
    tx.begin();
    for (auto &g :
         tx.query<Goods, "SELECT * FROM goods WHERE title = 'hello?'">()) {
      std::cout << g.title << std::endl;
    };
    tx.commit();
  } catch (...) {
    // logger.push
    tx.rollback();
  }

  // auto res = ess::orm::query<Goods, "SELECT * FROM goods">();

  transaction<Write>([](auto &txs) {
    transaction<Read, LoggerDB>([](auto &tx) {
      auto res = tx.template query<Log, "SELECT * FROM log">();
      for (auto &l : res) {
        std::cout << l.id << std::endl;
      }
    });

    transaction<Write>([](auto &tx) {
      auto res = tx.template query_rows<Goods, "SELECT * FROM goods">();
      for (auto &g : res) {
        std::cout << g["id"].template as<int>() << std::endl;
      }
    });
  });
  return 0;
}

void test_row() {
  ConnectionPool pool = ConnectionPool(config::MainDB::connection_url, 10);
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

void test_lexer_tokenize() {
  using namespace sql;
  // =========================================
  // 测试1：正常的所有 Token 混合
  // =========================================
  constexpr auto input1 =
      ", . * ( ) + - / ? = < <= <> > >= != hello SELECT INSERT FROM"_fs;
  constexpr LexResult result1 = Lexer(input1).tokenize<128>();
  // 检查没有错误
  static_assert(!result1.has_error, "Mixed tokens should have no error");
  // 检查 Token 数量
  // , . * ( ) + - / ? = < <= <> > >= != -> 18个
  // hello SELECT INSERT FROM -> 4个
  // END -> 1个
  // 总计: 18 + 4 + 1 = 23个
  static_assert(result1.count == 21, "Mixed tokens should produce 23 tokens");
  // 逐个检查 Token 类型
  // 第一行是特殊符号
  static_assert(result1[0].type == TokenType::COMMA);
  static_assert(result1[1].type == TokenType::DOT);
  static_assert(result1[2].type == TokenType::STAR);
  static_assert(result1[3].type == TokenType::LPAREN);
  static_assert(result1[4].type == TokenType::RPAREN);
  static_assert(result1[5].type == TokenType::PLUS);
  static_assert(result1[6].type == TokenType::MINUS);
  static_assert(result1[7].type == TokenType::SLASH);
  static_assert(result1[8].type == TokenType::PLACEHOLDER);
  static_assert(result1[9].type == TokenType::EQ);
  static_assert(result1[10].type == TokenType::LT);
  static_assert(result1[11].type == TokenType::LE);
  static_assert(result1[12].type == TokenType::NE);
  static_assert(result1[13].type == TokenType::GT);
  static_assert(result1[14].type == TokenType::GE);
  static_assert(result1[15].type == TokenType::NE); // `!=` 也被识别为 NE
  static_assert(result1[16].type == TokenType::IDENTIFIER); // hello
  static_assert(result1[17].type == TokenType::SELECT);
  static_assert(result1[18].type == TokenType::INSERT);
  static_assert(result1[19].type == TokenType::FROM);
  // 检查结尾是 END
  static_assert(result1[20].type == TokenType::END);
  // 超出数量的访问也应该是 END
  static_assert(result1[21].type == TokenType::END);
  // =========================================
  // 测试2：空输入
  // =========================================
  constexpr auto input2 = ""_fs;
  constexpr LexResult result2 = Lexer(input2).tokenize<128>();
  static_assert(!result2.has_error, "Empty input should have no error");
  static_assert(result2.count == 1,
                "Empty input should produce only END token");
  static_assert(result2[0].type == TokenType::END, "First token should be END");
  // =========================================
  // 测试3：只包含空白字符
  // =========================================
  constexpr auto input3 = " \t\n\r "_fs;
  constexpr LexResult result3 = Lexer(input3).tokenize<128>();
  static_assert(!result3.has_error,
                "Whitespace-only input should have no error");
  static_assert(result3.count == 1,
                "Whitespace should be ignored, only END token");
  static_assert(result3[0].type == TokenType::END);
  // =========================================
  // 测试4：空字符串
  // =========================================
  constexpr auto input4 = "''"_fs;
  constexpr LexResult result4 = Lexer(input4).tokenize<128>();
  static_assert(!result4.has_error, "Empty string should be valid");
  static_assert(result4.count == 2, "Empty string should be STRING + END");
  static_assert(result4[0].type == TokenType::STRING,
                "First token should be STRING");
  static_assert(result4[1].type == TokenType::END,
                "Second token should be END");
  // =========================================
  // 测试5：包含转义符的字符串
  // =========================================
  constexpr auto input5 = "'It''s a book'"_fs;
  constexpr LexResult result5 = Lexer(input5).tokenize<128>();
  static_assert(!result5.has_error,
                "String with escaped quote should be valid");
  static_assert(result5.count == 2, "Escaped string should be STRING + END");
  static_assert(result5[0].type == TokenType::STRING,
                "First token should be STRING");
  static_assert(result5[1].type == TokenType::END,
                "Second token should be END");
  // =========================================
  // 测试6：未闭合的字符串（触发错误）
  // =========================================
  constexpr auto input6 = "'hello"_fs;
  constexpr LexResult result6 = Lexer(input6).tokenize<128>();
  static_assert(result6.has_error, "Unterminated string should have an error");
  static_assert(result6.count == 1, "Should produce one UNKNOWN token");
  static_assert(result6[0].type == TokenType::UNKNOWN,
                "First token should be UNKNOWN");
  static_assert(result6.err_len == 6, "Error message has set");
  // =========================================
  // 测试7：输入字符串后跟非法字符 (你的老问题)
  // =========================================
  // 根据你之前的讨论，"''hello'" 应该被解析为 [STRING '', IDENTIFIER hello,
  // UNKNOWN ']
  constexpr auto input7 = "''hello'"_fs;
  constexpr LexResult result7 = Lexer(input7).tokenize<128>();
  static_assert(result7.has_error, "Input like ''hello' should have an error");
  static_assert(result7.count == 3, "Should produce 3 tokens");
  static_assert(result7[0].type == TokenType::STRING,
                "First token should be STRING");
  static_assert(result7[1].type == TokenType::IDENTIFIER,
                "Second token should be IDENTIFIER");
  static_assert(result7[2].type == TokenType::UNKNOWN,
                "Third token should be UNKNOWN");
  static_assert(result7.err_len == 1, "Error message has set");
  // =========================================
  // 测试8：数字与非法的尾随点
  // =========================================
  constexpr auto input8 = "1."_fs;
  constexpr LexResult result8 = Lexer(input8).tokenize<128>();
  static_assert(result8.has_error,
                "Number with a trailing dot (like '1.') should be invalid");
  static_assert(result8.count == 1, "Should be NUMBER + END");
  static_assert(result8[0].type == TokenType::UNKNOWN,
                "First token should be NUMBER");
  static_assert(result8[1].type == TokenType::END,
                "Second token should be END");
  // =========================================
  // 测试9：两个点 (Illegal)
  // =========================================
  constexpr auto input9 = ".."_fs;
  constexpr LexResult result9 = Lexer(input9).tokenize<128>();
  static_assert(result9.has_error, "'..' should be an error");
  static_assert(result9.count == 1, "Should produce one UNKNOWN token");
  static_assert(result9[0].type == TokenType::UNKNOWN,
                "First token should be UNKNOWN");
  static_assert(result9.err_len == 2, "Error message should be set");
}
