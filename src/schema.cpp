#include <core.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/row.hpp>
#include <ess/orm/runtime.hpp>
// #include <ess/orm/test/stress_test.hpp>
#include <sqlite3.h>

using namespace ess::orm;
using namespace ess::orm::meta;

enum class GoodsStatus : int { Normal = 0, Disabled, Deleted };

struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;

  using Schema = dsl::Schema<
      "goods",
      dsl::Field<"id", &Goods::id, attribute::PrimaryKey,
                 attribute::AutoIncrement, attribute::DefaultValue<1>>,
      dsl::Field<"title", &Goods::title,
                 attribute::DefaultValue<"untitled"_fs>>,
      dsl::Field<"price", &Goods::price, attribute::DefaultValue<0.0>>,
      dsl::Field<"stock", &Goods::stock, attribute::DefaultValue<0>>,
      dsl::Field<"status", &Goods::status,
                 attribute::DefaultValue<GoodsStatus::Deleted>>,
      dsl::Field<"enabled", &Goods::enabled, attribute::DefaultValue<true>>>;
};

template <size_t N> void println(const ess::orm::meta::FixedString<N> &str) {
  fmt::println("{}", std::string_view(str));
}

struct Foo {};

void func() {
  sqlite3 *db = nullptr;
  int rc = sqlite3_open_v2("data/test.db", &db, SQLITE_OPEN_READWRITE, nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "无法打开数据库: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return;
  }

  sqlite3_stmt *stmt;
  auto ddl = Goods::Schema::make_create_table_ddl();

  // fmt::println("{}", ddl);
  rc = sqlite3_prepare_v2(db, "SELECT * FROM goods WHERE id = 1", -1, &stmt,
                          nullptr);
  if (rc != SQLITE_OK) {
    std::cerr << "select stmt prepare failed: " << sqlite3_errmsg(db)
              << std::endl;
    sqlite3_close(db);
    return;
  }

  Goods goods{};

  int res = sqlite3_step(stmt);
  if (res == SQLITE_ROW) {
    auto mapper = ResultSetMapper<Goods>{};
    mapper.init_mapper(stmt);
    auto row = mapper.map_row(stmt);
    fmt::println("{} {} {} {} {} {}", (int)row["id"], (std::string)row["title"],
                 (double)row["price"], (double)row["stock"], (int)row["status"],
                 (bool)row["enabled"]);
    mapper.map_row(stmt, goods);
    fmt::println("{} {} {} {} {} {}", goods.id, goods.title, goods.price,
                 goods.stock, (int)goods.status, goods.enabled);
  }

  // ess::orm::config::print_config();

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

int main() {
  Goods goods{};

  constexpr auto idx = SchemaMapper<Goods>::find_field_index("status");
  fmt::println("{}", idx);

  func();

  // auto goods_ddl = Goods::Schema::make_create_table_ddl();
  // fmt::print(fmt::fg(fmt::color::aquamarine), "--- Goods DDL ---\n{}\n\n",
  //            goods_ddl);

  // ess::orm::query<Goods, "SELECT * FROM goods WHERE id > ?">(10);

  // fmt::println("{}", config::config::connection_url);
  // fmt::println("{}", config::config::password);

  // transaction([] {
  //   // 这两条语句会自动使用同一个 Connection 句柄
  //   // 且处于同一个 BEGIN...COMMIT 块中
  //   query<Goods, "UPDATE goods SET stock = stock - 1 WHERE id = ?">(101);
  //   query<Goods, "INSERT INTO logs (msg) VALUES (?)">("Stock updated");
  // });
  //
  // transaction([](auto &ctx) {
  //   // 通过 ctx 句柄保证连接一致性
  //   auto table = ctx.table<Goods>();
  //   table.update(g.stock = g.stock - 1).where(g.id == 101);
  // });

  return 0;
}
