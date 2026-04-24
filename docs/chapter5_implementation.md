# 第5章 系统实现

本章详细介绍ESS-ORM各核心模块的实现细节。开发环境采用C++20标准、CMake构建系统和SQLite3数据库。各模块的实现遵循模块化设计原则，充分利用C++20的模板元编程和constexpr特性实现编译期校验能力。

## 5.1 开发环境

## 5.1 开发环境

ESS-ORM的开发环境配置要求较高，因为使用了C++20的多项特性。具体环境配置如下：操作系统支持Linux和macOS；编译器需要GCC 12.0+或Clang 15+；C++标准必须使用C++20，因为需要constexpr、consteval等特性；构建工具使用CMake 3.21+，以确保正确解析C++20的模块特性；默认数据库为SQLite3，需要预先安装相应的开发库。

## 5.2 核心实现

ESS-ORM的整体代码组织结构如orm.hpp所示，该头文件作为主入口，包含了所有公共API。库按照功能划分为核心模块、配置系统、DSL定义、SQL处理和工具类五个部分。

### 5.2.1 DSL实现

**Field模板实现**

Field模板位于dsl/field.hpp，定义如代码所示。模板接受三个参数：ColumnName是FixedString类型的列名，在编译期确定；Ptr是成员指针，通过auto自动推断，建立C++成员变量与数据库字段的绑定关系；Attrs是属性列表，使用可变参数模板接受PrimaryKey、AutoIncrement等属性。

Field模板内部使用MemberPointerTraits提取成员指针的类型信息，得到pointer_type（指针类型）、member_type（成员类型）和class_type（所属类类型）。column_name和pointer都声明为static constexpr，确保编译期可用。attributes使用std::tuple存储属性列表。

核心的校验机制在_check私有成员函数中实现。调用attribute::check_attributes进行属性合法性检查，该函数使用static_assert在编译期检查属性与成员类型的匹配性。第二个static_assert检查has_dup_attrs_in_tuple，防止同类别属性的重复。错误信息明确指出三类不允许的情况：任意属性不能重复声明、DefaultValue与DefaultExpr互斥、不可存在多个默认值属性。

Field模板还提供了两个辅助工具：is_field特化用于检测类型是否为Field；field_type概念用于约束模板参数；is_same_binding函数用于检测两个Field是否绑定了相同的成员变量。

**Schema模板实现**

Schema模板位于dsl/schema.hpp，定义如代码所示。Schema模板接受表名和Field列表作为参数，使用no_duplicated_key_field_words约束确保没有重复字段。

no_duplicate_detector模板是编译期检测的核心，通过递归方式检测字段名是否重复。该模板继承自std::bool_constant，检测逻辑为：对于每对字段，比较其column_name和绑定的成员指针，如果都相同则返回false表示存在重复。

Schema模板提供两个关键成员函数：make_col_def生成单个字段的列定义，遍历Field的属性元组，将属性转换为SQL片段并拼接；make_create_table_ddl生成完整的CREATE TABLE语句，使用参数控制是否添加IF NOT EXISTS子句。

**字段属性实现**

字段属性定义在dsl/attribute.hpp中，采用分层设计。基类AttributeTag作为所有属性的公共基类。每个属性都是模板结构体，继承自AttributeTag。PrimaryKey、Unique、AutoIncrement、NotNull是简单属性。SerializedName<T>是模板属性，支持自定义序列化名称。DefaultValue<V>使用auto模板参数接收默认值。DefaultExpr<E>使用FixedString接收SQL表达式。

属性校验通过valid_attribute_semantic模板实现，每个属性针对不同的SQL语义类型有不同的合法性规则。例如，AutoIncrement仅对sql_integer语义类型合法；DefaultValue的值类型必须与字段的SQL语义类型兼容。

### 5.2.2 编译期SQL解析实现

编译期SQL解析利用C++20的constexpr特性实现，将传统的运行时解析提前到编译期。

**Token定义**

Token定义位于sql/token.hpp。TokenType枚举定义了所有可能的Token类型，包括SQL关键字（Select、From、Where等）、标识符、字面量（Number、String、PlaceHolder）、运算符（Eq、Ne、Lt等）、分隔符（Comma、Dot等）、以及辅助类型（End、Unknown）。每个Token包含类型、位置、长度、行号和列号信息，便于错误定位。

**Lexer实现**

Lexer模板位于sql/lexer.hpp，定义如代码所示。Lexer使用FixedString<N>存储源SQL字符串，m_pos、m_line、m_col记录当前位置信息。

tokenize方法标记为constexpr，在编译期遍历SQL字符串识别Token。方法内部使用while循环，逐个调用next_token方法获取Token，放入结果数组。当遇到Unknown类型的Token或End类型的Token时退出循环。

next_token方法是词法分析的核心，使用状态机模式处理各类字符。单字符Token（如逗号、括号）直接映射；多字符Token（如<=、>=）需要预读下一个字符；标识符调用scan_identifier扫描；数字调用scan_number扫描；字符串调用scan_string扫描。scan_identifier方法还会调用lookup_keyword函数，将标识符与预定义的关键字列表进行匹配。

词法分析结果存储在LexResult<N>模板中，包含Token数组、Token计数、错误标志和错误信息。

**Parser与Validator实现**

Parser位于sql/parser.hpp，采用模块化设计，不同语句类型的解析逻辑分散在独立的实现文件中（parser_select.ipp、parser_insert.ipp等）。

Validator位于sql/validator.hpp，核心是SQL字面量运算符operator""_sql。该运算符标记为consteval，确保在编译期执行。运算符内部调用SqlParseResult的构造函数，触发Lexer和Parser执行。如果词法或语法分析失败，static_assert会捕获错误并报告。

### 5.2.3 事务实现

事务实现利用RAII机制确保事务资源的正确管理。

**Transaction模板实现**

Transaction模板位于core/transaction.hpp，定义如代码所示。模板接受transaction_mode（Read或Write）和database_type两个参数。

构造函数接受Connection引用作为参数，将事务与数据库连接关联。Transaction支持移动语义，不支持复制拷贝。移动构造和移动赋值后，原对象的m_conn置为nullptr，m_started和m_finished标记为true，避免重复操作。

析构函数自动调用rollback，确保事务在未提交的情况下自动回滚。rollback调用包裹在try-catch中，即使回滚失败也不抛出异常。

begin方法根据Mode执行不同的BEGIN语句。Read模式执行"BEGIN"延迟获取锁，Write模式执行"BEGIN IMMEDIATE"立即获取写锁。commit和rollback方法分别执行COMMIT和ROLLBACK语句，并标记m_finished为true。

Transaction模板提供了四个重载的query成员函数，支持带表类型和不带表类型两种查询方式，带静态SQL和解析后SQL两种参数形式。

### 5.2.4 连接池实现

连接池实现采用Loan模式管理连接生命周期，避免频繁创建和销毁连接带来的性能开销。

**ConnectionPool模板实现**

ConnectionPool模板位于core/connection_pool.hpp，定义如代码所示。模板参数是dialect_type（数据库方言）。

Loan类是连接的生命周期包装器，核心特性是析构函数自动调用release归还连接。Loan支持移动语义，不支持复制。重载了operator->和operator*，使得Loan对象可以直接当作Connection指针使用。

ConnectionPool使用std::deque存储预创建的连接，使用std::mutex和std::condition_variable实现线程安全的等待机制。acquire方法使用wait_for等待可用连接，超时时间设为5秒。当等待超时或连接池关闭时，抛出runtime_error异常。

release方法将连接归还连接池。归还前会将连接加入队列，并调用notify_one唤醒等待线程。

### 5.2.5 结果映射实现

结果映射实现支持静态映射和动态映射两种方式。

**Row动态映射实现**

Row类位于core/row.hpp，使用std::variant存储多种数据类型。DBValue是预定义的variant类型，可以存储int、long long、double、std::string、bool五种类型。

Proxy内部类是值访问的包装器。Proxy重载了类型转换操作符和as方法，支持类型安全的值访问。get_converted方法使用std::visit实现类型安全的转换：同类型直接返回；数值类型之间可以强制转换；其他情况抛出异常。

Row的operator[]方法按列名访问值，如果列不存在抛出std::out_of_range异常。get_if方法返回std::optional，如果列不存在或类型不匹配返回std::nullopt。add_column方法添加列数据。

**ResultSetMapper实现**

ResultSetMapper模板位于core/result_set_mapper.hpp，采用双版本设计。泛化版本（template<typename T = void>）返回Row对象实现动态映射；针对table_type的特例化版本返回Table类型对象实现静态映射。

SchemaMapper是静态映射的核心组件。编译期构建排序后的字段索引数组（sorted_field_infos），利用二分查找实现高效的字段定位。编译期构建函数指针数组（fill_funcs），每个元素是对应字段的填充函数。

map_row方法遍历结果列，根据列名查找到对应的Field索引，然后调用fill_funcs中对应的填充函数。如果找不到映射，抛出"unknown column"异常。

### 5.2.6 配置实现

配置系统位于config目录，采用检测机制支持用户自定义配置。

config.hpp使用__has_include预处理器指令检测用户配置文件（ess_orm_user_config.hpp）是否存在。如果存在则包含用户配置，否则使用默认的UserConfig空结构体。

GlobalConfigTrait模板合并用户配置和默认配置，用户配置优先。Config聚合了dialect、databases、default_db等配置项。

## 5.3 关键代码示例

本节展示ESS-ORM的关键代码使用方式。

**Schema定义示例**

定义Goods表，包含id（主键+自增）、title（有默认值）、price（有默认值）三个字段。

```cpp
struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;

  using Database = default_db;
  using Schema = dsl::Schema<
      "goods",
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>
  >;
};
```

**query查询���例**

执行带参数的查询，结果映射为Goods类型的向量。

```cpp
auto goods = query<Goods, "SELECT * FROM goods WHERE id > ?">(0);
```

**transaction闭包式写法**

在事务中执行查询，闭包正常返回时自动提交，抛出异常时自动回滚。

```cpp
transaction<Write>([](auto& txs) {
  auto res = txs.query<Goods, "SELECT * FROM goods">();
  for (auto& g : res) {
    std::cout << g.id << " " << g.title << std::endl;
  }
});
```

## 5.4 本章小结

本章介绍了ESS-ORM的核心实现。开发环境采用C++20、CMake、SQLite3。DSL实现通过Field与Schema模板实现编译期属性检查。SQL解析利用constexpr实现编译期词法分析与语法分析。事务通过Transaction模板类实现RAII管理。连接池采用Loan模式实现连接复用。结果映射支持静态与动态两种方式。配置通过模板特化实现类型安全。

各模块实现遵循模块化设计原则，便于维护与扩展。详细代码实现见附录A至附录F。

---

## 附录A DSL实现核心代码

```cpp
// Field模板核心实现
template <meta::FixedString ColumnName, auto Ptr, typename... Attrs>
struct Field {
private:
  using member_traits = traits::MemberPointerTraits<Ptr>;
public:
  using pointer_type = typename member_traits::pointer_type;
  using member_type = member_traits::member_type;
  using class_type = typename member_traits::class_type;
  static constexpr meta::FixedString column_name = ColumnName;
  static constexpr decltype(auto) pointer = member_traits::pointer;
  using attributes = std::tuple<Attrs...>;
private:
  static constexpr bool _check() {
    attribute::check_attributes<member_type, Attrs...>();
    return true;
  }
  static_assert(_check());
  static_assert(!attribute::has_dup_attrs_in_tuple<attributes>, "存在重复类型的属性");
};
```

## 附录B SQL解析核心代码

```cpp
// Lexer核心实现
template <std::size_t N> class Lexer {
  meta::FixedString<N> m_src;
  std::size_t m_pos = 0;
  std::size_t m_line = 1;
  std::size_t m_col = 1;
public:
  constexpr explicit Lexer(meta::FixedString<N> src) : m_src(src) {}
  template <std::size_t MaxTokens>
  [[nodiscard]] constexpr LexResult<MaxTokens> tokenize() {
    LexResult<MaxTokens> result;
    while (result.count < MaxTokens) {
      auto token = next_token();
      result.tokens[result.count++] = token;
      if (token.type == TokenType::Unknown) { result.has_error = true; break; }
      if (token.type == TokenType::End) break;
    }
    return result;
  }
private:
  constexpr Token next_token() { /* 状态机实现 */ }
};
```

## 附录C 事务核心代码

```cpp
// Transaction模板核心实现
template <transaction_mode Mode, concepts::database_type DB>
class Transaction {
public:
  explicit Transaction(Connection &conn) : m_conn(&conn) {}
  ~Transaction() noexcept { try { rollback(); } catch (...) {} }
  void begin() { if (m_started) return; m_conn->begin_transaction(mode); m_started = true; }
  void commit() { if (!m_started || m_finished) return; m_conn->commit(); m_finished = true; }
  void rollback() { if (!m_started || m_finished) return; m_conn->rollback(); m_finished = true; }
private:
  Connection *m_conn = nullptr;
  bool m_started = false;
  bool m_finished = false;
};
```

## 附录D 连接池核心代码

```cpp
// ConnectionPool核心实现
template <dialect::dialect_type Dialect>
class ConnectionPool : public std::enable_shared_from_this<ConnectionPool<Dialect>> {
public:
  class Loan {
  public:
    ~Loan() { if (m_pool_ref && m_conn) m_pool_ref->release(std::move(m_conn)); }
    Connection *operator->() const { return m_conn.get(); }
    Connection &operator*() const { return *m_conn.get(); }
  private:
    std::shared_ptr<ConnectionPool> m_pool_ref;
    ConnectionPtr m_conn;
  };
  [[nodiscard]] Loan acquire() {
    std::unique_lock lock(m_mutex);
    bool got = m_cv.wait_for(lock, std::chrono::seconds{5}, [this]() {
      return !m_pool.empty() || m_shutdown;
    });
    // ... 超时处理
    auto conn = std::move(m_pool.front());
    m_pool.pop_front();
    return Loan(this->shared_from_this(), std::move(conn));
  }
};
```

## 附录E 结果映射核心代码

```cpp
// ResultSetMapper静态映射核心实现
template <concepts::table_type Table> struct ResultSetMapper {
  std::vector<int> m_col_to_field_map{};
  using schema_mapper = SchemaMapper<Table>;
  void init_mapper(core::Statement &stmt) {
    int size = stmt.column_count();
    m_col_to_field_map.resize(size);
    for (int i = 0; i < size; ++i) {
      std::string name = stmt.column_name(i);
      m_col_to_field_map[i] = schema_mapper::find_field_index(name);
    }
  }
  Table map_row(core::Statement &stmt) {
    Table obj;
    for (int col_idx = 0; col_idx < m_col_to_field_map.size(); ++col_idx) {
      int field_idx = m_col_to_field_map[col_idx];
      if (field_idx == -1) throw std::runtime_error("unknown column");
      schema_mapper::fill_funcs[field_idx](obj, stmt, col_idx);
    }
    return obj;
  }
};
```

## 附录F 关键代码使用示例

```cpp
// Schema定义
struct Goods {
  long long id = 0;
  std::string title;
  float price = 0.0;
  using Database = default_db;
  using Schema = dsl::Schema<
      "goods",
      Field<"id", &Goods::id, PrimaryKey, AutoIncrement>,
      Field<"title", &Goods::title, DefaultValue<"untitled"_fs>>,
      Field<"price", &Goods::price, DefaultValue<0.0_fp>>
  >;
};

// 查询
auto goods = query<Goods, "SELECT * FROM goods WHERE id > ?">(0);

// 事务闭包
transaction<Write>([](auto& txs) {
  auto res = txs.query<Goods, "SELECT * FROM goods">();
  for (auto& g : res) {
    std::cout << g.id << " " << g.title << std::endl;
  }
});
```