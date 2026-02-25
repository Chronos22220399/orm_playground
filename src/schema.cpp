#include <core.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/connection_pool.hpp>
#include <ess/orm/context.hpp>
#include <ess/orm/parser.hpp>
#include <ess/orm/result_set_mapper.hpp>
#include <ess/orm/runtime.hpp>
// #include <ess/orm/sql/ast/common.hpp>
// #include <ess/orm/sql/ast/delete.hpp>
// #include <ess/orm/sql/ast/insert.hpp>
// #include <ess/orm/sql/ast/select.hpp>
// #include <ess/orm/sql/ast/statement.hpp>
// #include <ess/orm/sql/ast/storage.hpp>
// #include <ess/orm/sql/ast/update.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parser.hpp>
#include <ess/orm/statement.hpp>
#include <ess/orm/transaction.hpp>
// #include <ess/orm/test/stress_test.hpp>
#include <sqlite3.h>

using namespace ess::orm::config;
using namespace ess::orm::meta;
using namespace ess::orm::dsl;
using namespace ess::orm::attribute;

void test_row();
void test_multithread();
void init_database();
void test_row_asan_safety();
void test_lexer_tokenize();

enum class GoodsStatus : int { Normal = 0, Disabled, Deleted };

struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal; // enum
  bool enabled = true;

  using Database = default_db;
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

namespace ess::orm::sql {

template <meta::FixedString Sql, bool enable_sql_dynamic_check = false>
consteval auto query() {
  if constexpr (enable_sql_dynamic_check) {

  } else {
    constexpr LexResult<128> lex_res = Lexer(Sql).template tokenize<128>();
    static_assert(lex_res.tokens[0].type == TokenType::Select);
    static_assert(lex_res.tokens[1].type == TokenType::Identifier);
    static_assert(lex_res.tokens[2].type == TokenType::Comma);
    static_assert(lex_res.tokens[3].type == TokenType::Identifier);
    static_assert(lex_res.tokens[4].type == TokenType::From);
    static_assert(lex_res.tokens[5].type == TokenType::Identifier);
    static_assert(lex_res.tokens[lex_res.count - 1].type == TokenType::End);
    // check<lex_res>();

    constexpr auto parse_res = Parser(lex_res.tokens).parse();
    check<parse_res>();
    constexpr auto names = parse_res.column_names;

    constexpr size_t pos_1 = names[0].pos;
    constexpr size_t len_1 = names[0].len;
    constexpr size_t pos_2 = names[1].pos;
    constexpr size_t len_2 = names[1].len;
    static_assert(pos_1 == 7);
    static_assert(len_1 == 2);
    static_assert(fs_equal(fs_substr<7, 2>(Sql), "id"_fs));

    static_assert(pos_2 == 11);
    static_assert(len_2 == 4);
    static_assert(fs_equal(fs_substr<pos_2, len_2>(Sql), "name"_fs));
  }
  return 0;
}

} // namespace ess::orm::sql

// MARK: ============= Main ==============
/*
 *
 */
int main() {
  // auto res = query<"SELECT id, name FROM goods WHERE id > 10">();
  auto res = ess::orm::query<Goods, "SELECT * FROM goods">();
  std::string sql = "SELECT * FROM goods WHERE title = 'hello'";
  ess::orm::Context::instance().conn_pool().acquire()->execute_raw(sql);

  ess::orm::transaction(
      [](auto &tx) { tx.template query<Goods, "SELECT * FROM goods">(); });

  return 0;
}
/*
 *
 */
// ============= Main ==============

void test_row() {
  using namespace ess::orm;
  auto pool = ConnectionPool(MainDB::connection_url, 10);
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

void test_lexer_tokenize() {
  using namespace ess::orm::sql;
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
  static_assert(result1[0].type == TokenType::Comma);
  static_assert(result1[1].type == TokenType::Dot);
  static_assert(result1[2].type == TokenType::Star);
  static_assert(result1[3].type == TokenType::Lparen);
  static_assert(result1[4].type == TokenType::Rparen);
  static_assert(result1[5].type == TokenType::Plus);
  static_assert(result1[6].type == TokenType::Minus);
  static_assert(result1[7].type == TokenType::Slash);
  static_assert(result1[8].type == TokenType::PlaceHolder);
  static_assert(result1[9].type == TokenType::Eq);
  static_assert(result1[10].type == TokenType::Lt);
  static_assert(result1[11].type == TokenType::Le);
  static_assert(result1[12].type == TokenType::Ne);
  static_assert(result1[13].type == TokenType::Gt);
  static_assert(result1[14].type == TokenType::Ge);
  static_assert(result1[15].type == TokenType::Ne); // `!=` 也被识别为 NE
  static_assert(result1[16].type == TokenType::Identifier); // hello
  static_assert(result1[17].type == TokenType::Select);
  static_assert(result1[18].type == TokenType::Insert);
  static_assert(result1[19].type == TokenType::From);
  // 检查结尾是 END
  static_assert(result1[20].type == TokenType::End);
  // 超出数量的访问也应该是 END
  static_assert(result1[21].type == TokenType::End);
  // =========================================
  // 测试2：空输入
  // =========================================
  constexpr auto input2 = ""_fs;
  constexpr LexResult result2 = Lexer(input2).tokenize<128>();
  static_assert(!result2.has_error, "Empty input should have no error");
  static_assert(result2.count == 1,
                "Empty input should produce only END token");
  static_assert(result2[0].type == TokenType::End, "First token should be END");
  // =========================================
  // 测试3：只包含空白字符
  // =========================================
  constexpr auto input3 = " \t\n\r "_fs;
  constexpr LexResult result3 = Lexer(input3).tokenize<128>();
  static_assert(!result3.has_error,
                "Whitespace-only input should have no error");
  static_assert(result3.count == 1,
                "Whitespace should be ignored, only END token");
  static_assert(result3[0].type == TokenType::End);
  // =========================================
  // 测试4：空字符串
  // =========================================
  constexpr auto input4 = "''"_fs;
  constexpr LexResult result4 = Lexer(input4).tokenize<128>();
  static_assert(!result4.has_error, "Empty string should be valid");
  static_assert(result4.count == 2, "Empty string should be STRING + END");
  static_assert(result4[0].type == TokenType::String,
                "First token should be STRING");
  static_assert(result4[1].type == TokenType::End,
                "Second token should be END");
  // =========================================
  // 测试5：包含转义符的字符串
  // =========================================
  constexpr auto input5 = "'It''s a book'"_fs;
  constexpr LexResult result5 = Lexer(input5).tokenize<128>();
  static_assert(!result5.has_error,
                "String with escaped quote should be valid");
  static_assert(result5.count == 2, "Escaped string should be STRING + END");
  static_assert(result5[0].type == TokenType::String,
                "First token should be STRING");
  static_assert(result5[1].type == TokenType::End,
                "Second token should be END");
  // =========================================
  // 测试6：未闭合的字符串（触发错误）
  // =========================================
  constexpr auto input6 = "'hello"_fs;
  constexpr LexResult result6 = Lexer(input6).tokenize<128>();
  static_assert(result6.has_error, "Unterminated string should have an error");
  static_assert(result6.count == 1, "Should produce one UNKNOWN token");
  static_assert(result6[0].type == TokenType::Unknown,
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
  static_assert(result7[0].type == TokenType::String,
                "First token should be STRING");
  static_assert(result7[1].type == TokenType::Identifier,
                "Second token should be IDENTIFIER");
  static_assert(result7[2].type == TokenType::Unknown,
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
  static_assert(result8[0].type == TokenType::Unknown,
                "First token should be NUMBER");
  static_assert(result8[1].type == TokenType::End,
                "Second token should be END");
  // =========================================
  // 测试9：两个点 (Illegal)
  // =========================================
  constexpr auto input9 = ".."_fs;
  constexpr LexResult result9 = Lexer(input9).tokenize<128>();
  static_assert(result9.has_error, "'..' should be an error");
  static_assert(result9.count == 1, "Should produce one UNKNOWN token");
  static_assert(result9[0].type == TokenType::Unknown,
                "First token should be UNKNOWN");
  static_assert(result9.err_len == 2, "Error message should be set");
}
