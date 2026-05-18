// 基于examples/main.cpp的测试用例
// 使用共享内存数据库 file::memory:?cache=shared

#include <ess/orm/orm.hpp>
#include <gtest/gtest.h>
#include <iostream>

using namespace ess::orm;
using namespace ess::orm::dsl;
using namespace ess::orm::attribute;
using namespace ess::orm::meta;
using namespace ess::orm::sql;

// ==================== 测试数据库配置 ====================

struct TestDB {
  static constexpr std::string_view connection_url =
      "file::memory:?cache=shared";
  static constexpr std::size_t pool_size = 4;
};

// ==================== 测试表结构 ====================
enum class TestStatus : int { Active = 0, Inactive = 1, Deleted = 2 };

struct Goods {
  long long id{};
  std::string title{};
  float price{0.0};
  int stock{0};
  TestStatus status{TestStatus::Deleted};
  bool enabled{true};

  using Database = TestDB;
  using Schema = dsl::Schema<
      "goods", Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>,
      Field<"stock", &Goods::stock, DefaultValue<0>>,
      Field<"status", &Goods::status, DefaultValue<TestStatus::Deleted>>,
      Field<"enabled", &Goods::enabled, DefaultValue<true>>>;
};

struct TestTable {
  long long id{};
  std::string content{};

  using Database = TestDB;
  using Schema = dsl::Schema<"test", Field<"id", &TestTable::id, PrimaryKey>,
                             Field<"content", &TestTable::content>>;
};

// ==================== 测试夹具 ====================
class ExampleBasedTest : public ::testing::Test {
protected:
  void SetUp() override {
    // 注册测试数据库
    auto &ctx = Context::instance();
    ctx.register_db<TestDB>();

    // 清理并创建测试表
    auto goods_ddl = Goods::Schema::make_create_table_ddl();
    auto test_ddl = TestTable::Schema::make_create_table_ddl();

    auto &pool = ctx.conn_pool<TestDB>();
    auto conn = pool.acquire();

    // 先删除表（如果存在）
    conn->execute_raw("DROP TABLE IF EXISTS goods");
    conn->execute_raw("DROP TABLE IF EXISTS test");

    // 创建新表
    conn->execute_raw(goods_ddl);
    conn->execute_raw(test_ddl);

    // 插入基础测试数据
    query<Goods,
          "insert into goods (title, price, stock, status, enabled) values (?, "
          "?, ?, ?, ?)",
          TestDB>("Product 1", 19.99, 100, 0, true);
    query<Goods,
          "insert into goods (title, price, stock, status, enabled) values (?, "
          "?, ?, ?, ?)",
          TestDB>("Product 2", 29.99, 50, 0, true);
    query<Goods,
          "insert into goods (title, price, stock, status, enabled) values (?, "
          "?, ?, ?, ?)",
          TestDB>("Product 3", 9.99, 200, 1, false);

    query<TestTable, "insert into test values (?, ?)", TestDB>(1,
                                                               "Hello World");
    query<TestTable, "insert into test values (?, ?)", TestDB>(2,
                                                               "Test Content");
  }

  void TearDown() override {
    // 内存数据库，测试结束自动清理
  }
};

// ==================== 测试用例 ====================

// 测试1: 基础查询功能（基于示例第69-74行）
TEST_F(ExampleBasedTest, BasicQueryExample) {
  // 测试TestTable表的查询
  auto res = query<TestTable, "select * from test"_sql, TestDB>();
  EXPECT_EQ(res.size(), 2);

  EXPECT_EQ(res[0].id, 1);
  EXPECT_EQ(res[0].content, "Hello World");

  EXPECT_EQ(res[1].id, 2);
  EXPECT_EQ(res[1].content, "Test Content");
}

// 测试2: 带表类型的查询（基于示例第79-82行）
TEST_F(ExampleBasedTest, TableTypeQueryExample) {
  auto res = query<Goods, "select * from goods order by id"_sql, TestDB>();
  EXPECT_EQ(res.size(), 3);

  EXPECT_EQ(res[0].id, 1);
  EXPECT_EQ(res[0].title, "Product 1");
  EXPECT_FLOAT_EQ(res[0].price, 19.99f);
  EXPECT_EQ(res[0].stock, 100);
  EXPECT_EQ(res[0].status, TestStatus::Active);
  EXPECT_TRUE(res[0].enabled);

  EXPECT_EQ(res[2].id, 3);
  EXPECT_EQ(res[2].title, "Product 3");
  EXPECT_FLOAT_EQ(res[2].price, 9.99f);
  EXPECT_EQ(res[2].stock, 200);
  EXPECT_EQ(res[2].status, TestStatus::Inactive);
  EXPECT_FALSE(res[2].enabled);
}

// 测试3: 参数化查询（基于示例第87-89行）
TEST_F(ExampleBasedTest, ParameterizedQueryExample) {
  // 使用参数化查询
  auto res = query<Goods, "select * from goods where id = ?"_sql, TestDB>(2);
  EXPECT_EQ(res.size(), 1);

  if (!res.empty()) {
    EXPECT_EQ(res[0].id, 2);
    EXPECT_EQ(res[0].title, "Product 2");
    EXPECT_FLOAT_EQ(res[0].price, 29.99f);
  }
}

// 测试4: 不带表类型的查询
TEST_F(ExampleBasedTest, QueryWithoutTableType) {
  // 使用不带表类型的查询
  auto res = query<"select count(*) as cnt from goods"_sql, TestDB>();
  EXPECT_EQ(res.size(), 1);
  EXPECT_EQ(static_cast<int>(res[0]["cnt"]), 3);

  // 测试原始SQL查询
  auto res2 = query<"select id, title from goods where id = ?"_sql, TestDB>(1);
  EXPECT_EQ(res2.size(), 1);
  EXPECT_EQ(static_cast<int>(res2[0]["id"]), 1);
  EXPECT_EQ(static_cast<std::string>(res2[0]["title"]), "Product 1");
}

// 测试5: 事务测试（基于示例第84-90行）
TEST_F(ExampleBasedTest, TransactionExample) {
  int row_count = 0;

  transaction<core::Write, TestDB>([&](auto &tx) {
    // 事务内查询
    auto rows =
        tx.template query<Goods, "select * from goods where id = ?"_sql>(1);
    row_count = rows.size();

    // 事务内插入
    tx.template query<TestTable, "insert into test values (?, ?)"_sql>(
        3, "Transaction Test");
  });

  EXPECT_EQ(row_count, 1);

  // 验证事务提交
  auto res = query<TestTable, "select * from test where id = 3"_sql, TestDB>();
  EXPECT_EQ(res.size(), 1);
  EXPECT_EQ(res[0].content, "Transaction Test");
}

// 测试6: 事务回滚
TEST_F(ExampleBasedTest, TransactionRollback) {
  // 获取初始计数
  auto initial = query<"select count(*) as cnt from test"_sql, TestDB>();
  int initial_count = static_cast<int>(initial[0]["cnt"]);

  try {
    transaction<core::Write, TestDB>([&](auto &tx) {
      tx.template query<TestTable, "insert into test values (?, ?)"_sql>(
          999, "Rollback Test");
      throw std::runtime_error("Test exception for rollback");
    });
  } catch (const std::exception &) {
    // 预期异常
  }

  // 验证回滚
  auto final = query<"select count(*) as cnt from test"_sql, TestDB>();
  int final_count = static_cast<int>(final[0]["cnt"]);
  EXPECT_EQ(final_count, initial_count);

  // 验证记录不存在
  auto check =
      query<"select count(*) as cnt from test where id = 999"_sql, TestDB>();
  EXPECT_EQ(static_cast<int>(check[0]["cnt"]), 0);
}

// 测试7: 连接池测试
TEST_F(ExampleBasedTest, ConnectionPoolTest) {
  // 多次查询验证连接池工作正常
  for (int i = 0; i < 10; i++) {
    auto res = query<"select count(*) as cnt from goods"_sql, TestDB>();
    EXPECT_EQ(res.size(), 1);
    EXPECT_EQ(static_cast<int>(res[0]["cnt"]), 3);
  }
}

// 测试8: 插入操作测试
TEST_F(ExampleBasedTest, InsertOperation) {
  // 测试插入操作
  auto result =
      query<Goods,
            "insert into goods (title, price, stock) values (?, ?, ?)"_sql,
            TestDB>("New Product", 49.99, 25);

  // 验证插入
  auto res = query<Goods, "select * from goods where title = 'New Product'"_sql,
                   TestDB>();
  EXPECT_EQ(res.size(), 1);
  EXPECT_EQ(res[0].title, "New Product");
  EXPECT_FLOAT_EQ(res[0].price, 49.99f);
  EXPECT_EQ(res[0].stock, 25);
}

// 测试9: 更新操作测试
TEST_F(ExampleBasedTest, UpdateOperation) {
  // 更新记录
  query<Goods, "update goods set price = ? where id = ?"_sql, TestDB>(39.99, 1);

  // 验证更新
  auto res = query<Goods, "select * from goods where id = 1"_sql, TestDB>();
  EXPECT_EQ(res.size(), 1);
  EXPECT_FLOAT_EQ(res[0].price, 39.99f);
}

// 测试10: 删除操作测试
TEST_F(ExampleBasedTest, DeleteOperation) {
  // 删除记录
  query<Goods, "delete from goods where id = ?"_sql, TestDB>(3);

  // 验证删除
  auto res = query<Goods, "select * from goods where id = 3"_sql, TestDB>();
  EXPECT_TRUE(res.empty());

  // 验证剩余记录数
  auto count = query<"select count(*) as cnt from goods"_sql, TestDB>();
  EXPECT_EQ(static_cast<int>(count[0]["cnt"]), 2);
}

// ==================== 主函数 ====================
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(color) = "yes";
  return RUN_ALL_TESTS();
}
