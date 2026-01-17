#include <core.hpp>
#include <ess/orm/config/config.hpp>
#include <ess/orm/result_set_mapper.hpp>
#include <ess/orm/row.hpp>
#include <ess/orm/statement.hpp>
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

  auto ddl = Goods::Schema::make_create_table_ddl();

  Statement stmt;
  stmt.prepare(db, "SELECT * FROM goods WHERE id > ?");
  stmt.bind_params(0);

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
    // auto price = row.get_if<double>("price").value();

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

  stmt.clear_bindings();

  // ess::orm::config::print_config();

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
