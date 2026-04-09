#import "@preview/polylux:0.4.0": *
#import "@preview/helios-polylux:0.1.0": *

// ============================================
// 全局设置（修复字体）
// ============================================
#show: setup.with(
  text-font: "PingFang SC",
  math-font: "Songti SC",
  code-font: "Menlo",
)

#set text(
  font: ("PingFang SC", "Heiti SC"),
  lang: "zh",
)

// ============================================
// 封面 - 现代分栏风格
// ============================================
#slide[
  #set page(header: none, footer: none, margin: 0pt)

  // 左侧深色区域
  #place(left + horizon)[
    #rect(
      width: 35%,
      height: 100%,
      fill: rgb("#003366"),
    )
  ]

  // 左侧 Logo（白色/浅色效果）
  #place(left + horizon, dx: 5%, dy: 0%)[
    #box(width: 25%)[
      // 如果 logo 是深色，可以加滤镜或用白色版本
      #image("./assets/hbut.png", width: 100%)
    ]
  ]

  // 左下角学校名
  #place(left + bottom, dx: 4%, dy: -8%)[
    #text(
      size: 0.9em,
      weight: "bold",
      fill: white,
      tracking: 2pt,
    )[湖北工业大学]
  ]

  // 右侧内容区
  #place(right + horizon, dx: -5%)[
    #box(width: 55%)[
      #set align(left)

      // 类型标签
      #text(
        size: 0.85em,
        weight: "medium",
        fill: rgb("#003366"),
        tracking: 2pt,
      )[
        本科毕业设计 · 开题答辩
      ]

      #v(1.5em)

      // 主标题
      #text(
        size: 1.6em,
        weight: "bold",
        fill: rgb("#1a1a1a"),
      )[
        基于 C++20 的静态 ORM 库设计与实现
      ]

      #v(0.8em)

      // 英文副标题
      #text(
        size: 0.8em,
        fill: gray,
      )[
        Design and Implementation of a Static ORM Library Based on C++20
      ]

      #v(3em)

      // 分隔线
      #line(length: 40%, stroke: 1.5pt + rgb("#003366"))

      #v(2em)

      // 信息
      #set text(size: 0.95em)
      #grid(
        columns: (auto, auto),
        row-gutter: 1.2em,
        column-gutter: 1em,

        text(fill: gray)[答辩人], [*杨康*],
        text(fill: gray)[指导教师], [*李辰* ],
        text(fill: gray)[专业班级], [计算机科学与技术职师2班],
      )

      #v(3em)

      #text(size: 0.85em, fill: gray)[
        2025年2月3日
      ]
    ]
  ]
]

// ============================================
// 目录
// ============================================
#slide[
  = 汇报提纲

  #set text(size: 1.2em)

  #v(1em)

  #enum(
    numbering: "一、",
    tight: false,
    spacing: 1.5em,
    [国内外研究现状],
    [选题背景与意义],
    [研究内容与目标],
    [实施方案],
    [进度安排],
    [已查阅的参考文献],
  )
]

// ============================================
// 第一部分：国内外研究现状
// ============================================
#make-section[国内外研究现状]

#slide[
  = 国内外研究进展：工业化与编译期演进

  #table(
    columns: (auto, 1fr, auto),
    inset: 8pt,
    align: horizon,
    stroke: 0.5pt + gray,
    table.header([*库名称*], [*技术特点与研究贡献*], [*状态/地位*]),

    [#link("https://www.codesynthesis.com/products/odb/")[ODB]],
    [采用特定的编译器驱动（Compiler Driver）分析头文件，生成映射代码。支持复杂的对象生命周期管理。],
    [工业级成熟],

    [#link(
      "https://www.webtoolkit.eu/wt/doc/reference/html/group__dbo.html",
    )[Wt::Dbo]],
    [受 Boost.Serialization 启发，使用 `persist` 函数进行双向映射。深度集成于 Web 框架，强调易用性。],
    [经典框架],

    [#link("https://github.com/rbock/sqlpp11")[sqlpp11]],
    [#strong[里程碑式作品]。利用表达式模板（Expression Templates）实现类型安全的 SQL DSL，减少运行时错误。],
    [C++11 标杆],

    [#link("https://github.com/p-ranav/constexpr-sql")[constexpr-sql]],
    [#strong[极致探索]。在 C++20 环境下，通过递归模板解析 SQL 字符串，将 SQL 验证完全提前至编译阶段。],
    [前沿探索],
  )

  #v(5pt)
  #set text(size: 18pt)
  *结论*：国内外研究已实现从“反射模拟”向“编译期 DSL”的转变，核心趋势是利用 C++ 强类型系统消除 SQL 注入和运行时逻辑错误。
]


// ============================================
// 第二部分：选题背景与意义
// ============================================
#make-section[选题背景与意义]

#slide[
  = 研究背景

  == 现实需求

  - 大部分`C++`的ORM库无法兼顾易用性与安全性
    - 如`sqlpp11`、`odb`采用外部生成代码的方式，与迅捷开发相悖
    - `ORMpp`、`Wt::Dbo`缺少对原生`SQL`字符串的编译期校验，而其库本身不足以让用户彻底脱离`SQL`
  - 大部分`C++`的`ORM`库与其他生态严重分割，使用极为不便
    - 通过这些库得到的结果要与其他库结合起来需要大量麻烦的特化
    - 如`sqlpp11`得到的结果无法自动序列化，须重复编写大量胶水代码

  #v(1em)

  == 技术驱动

  - 开发效率与运行安全的平衡
    - 编译期计算技术的演变为编译期语法、语义校验提供了可能
    - 现代`C++`的发展逐渐往模版元编程方向靠拢，从最初的技巧使用到现在逐步融入语言标准
]

#slide[
  = 研究意义：理论创新

  #grid(columns: (1fr, 1fr), gutter: 2em)[
    #hypothesis[
      理论意义
    ][
      - 探索C++编译期优化策略
        - 利用C++编译期迭代技术降低编译栈深度
        - 在底层解耦模版逻辑，缓解二进制文件膨胀
        - 实现编译期元数据的常数级检索
      - 研究编译期`SQL`解析与`ORM`结合的新方案
        - 利用静态解析的元数据优化映射
    ]
  ][
    #hypothesis(accent: rgb("#099D72"))[
      实践意义
    ][
      - 为相关设计提供参考
        - 在传统`ORM`的基础上提供了新的设计方案
        - 基于`__has_include`实现了非侵入式配置方案
      - 降低 C++ 数据持久化开发成本
        - 提供无感知`API`，使用户仅需关注业务
        - 在保证性能的基础上，保证用户多样化需求
    ]
  ]
]


// ============================================
// 第三部分：研究内容与目标
// ============================================
#make-section[研究内容与目标]

#slide[
  = 研究内容

  #grid(columns: (1fr, 1fr), gutter: 2em)[
    == 核心任务

    #set text(size: 14pt)
    + *DSL 构建*
      - 设计符合`SQL`语义的`DSL`
      - 提供类型安全的`DSL`语义校验

    + *SQL 功能封装与映射实现*
      - 封装 SQL 模块
      - 实现映射机制

    + *SQL 语义校验器实现*
      - 实现词法分析
      - 实现语法分析
      - 实现结合DSL的语义分析
  ][
    == 研究目标

    #rect(
      width: 100%,
      fill: rgb("#f0f8ff"),
      stroke: rgb("#003366"),
      radius: 5pt,
      inset: 1.2em,
    )[
      #set text(size: 14pt)
      - *静态校验*：SQL 语义与 Schema *100%* 静态匹配，编译期拦截语法/类型错误。

      - *自动管理*：基于 `RAII` 实现事务传播与连接池回收，降低资源管理成本。

      - *多库路由*：通过 `Database` 标签实现物理库静态隔离，保障事务安全性。

      - *性能优化*：零堆内存分配（Zero-Allocation），且复杂 Schema 编译增量 < *2s*。

      - *解耦扩展*：非侵入式 `Field` 映射，元数据可无缝对接第三方序列化库。
    ]
  ]
]

// 目标成果演示：非侵入式映射与语义定义
#slide[
  = 目标成果演示：非侵入式映射与语义定义

  #grid(columns: (1.2fr, 1fr), gutter: 1em)[
    #set text(size: 13pt)
    ```cpp
    // 纯粹的业务实体 (POD)
    struct Goods {
      long long id = 0;
      std::string title;
      float price = 0.0;
      int stock = 0;
      GoodsStatus status = GoodsStatus::Normal; // enum
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
    ```
  ][
    #v(2em)
    #rect(fill: rgb("#f9f9f9"), inset: 1em, radius: 4pt)[
      *亮点解析：*
      - *零继承*：`Goods` 不需要继承任何基类。
      - *元数据分离*：通过成员指针映射，业务逻辑与持久化逻辑完全解耦。
      - *编译期常量*：`"goods"` 是 NTTP 字符串，支持后续解析。
      - *高复用性*: 可通过类型萃取复用`Schema`
    ]
  ]
]

#slide[
  = 目标成果演示：基于RAII与LTS的事务管理
  #grid(columns: (1.2fr, 1fr), gutter: 1em)[
    #set text(size: 14pt)
    ```cpp
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
    ```
  ][
    #v(2em)
    #rect(fill: rgb("#f9f9f9"), inset: 1em, radius: 4pt)[
      *亮点解析：*
      - *隐式上下文管理*：支持多库嵌套事务，上下文自动管理。
      - *静态隔离*: 通过类型系统实现不同数据库的事务隔离
      - *读写事务分离*：区分读写事务，并提供校验禁止不合理用法。
      - *自动类型映射*：无需处理繁琐的结果映射逻辑，库提供了两套映射方式。
    ]
  ]
]

#slide[
  = 目标成果演示：精准的编译期错误追踪

  #v(0.5em)
  #text(
    size: 14pt,
  )[当开发者编写了非法的 SQL 语句时，解析器会通过 `static_assert` 抛出详细信息：]

  #v(1em)
  #rect(
    width: 100%,
    fill: rgb("#1e1e1e"), // 深色背景模拟终端
    stroke: rgb("#c53030"),
    radius: 4pt,
    inset: 1.5em,
  )[
    #set text(font: "JetBrainsMono NF", size: 12pt, fill: rgb("#ffffff"))
    #set par(leading: 0.6em)

    // 模拟代码
    #text(fill: rgb("#dcdcdc"))[constexpr auto sql = "SELECT INSERT !"]#text(
      fill: rgb("#569cd6"),
    )[\_fs;] \

    #v(0.8em)
    #text(
      fill: rgb("#f44336"),
      weight: "bold",
    )[[Ess-Orm] Static Parser Error:] \
    #text(fill: rgb("#ffeb3b"))[Line 1, Column 16:] \

    #v(0.5em)
    #h(2em) SELECT INSERT ! \
    #h(10.4em) #text(fill: rgb("#f44336"))[                ^] \

    #v(0.5em)
    #text(fill: rgb("#f44336"))[Unexpected Token: '!' (Unsupported Character)] \
    #text(
      fill: rgb("#90caf9"),
    )[Expected: IDENTIFIER, STAR, or DISTINCT after SELECT.]
  ]
]

#slide[
  = 目标成果演示：关键性能指标 (Benchmark)

  #v(0.5em)
  #text(
    size: 14pt,
  )[我使用 Google Benchmark 对 `ESS-ORM` 与`Sqlite3原生驱动`在关键操作上进行了性能对比。]

  #v(1em)
  #align(center)[
    #image("./assets/benchmark_part.png", width: 100%) // 这里会替换为生成的图片
  ]
]

// ============================================
// 第四部分：实施方案
// ============================================
#make-section[实施方案]

#slide[
  = 技术路线 (按实际研究阶段)

  #align(center)[
    #let step(content) = rect(
      fill: rgb("#e6f2ff"),
      stroke: rgb("#003366"),
      radius: 5pt,
      inset: (x: 1.5em, y: 0.5em),
    )[#content]

    #stack(
      dir: ttb,
      spacing: 0.2em,
      step[*DSL 定义与 Schema 静态映射实现*],
      [↓],
      step[*模板编译优化 (解决爆内存与深度实例化问题)*],
      [↓],
      step[*运行时框架构建 (连接池/多库路由/嵌套事务)*],
      [↓],
      step[*事务安全性分析 (解决死锁/读写事务分离)*],
      [↓],
      step[*性能基准测试 (基于 Google Benchmark 对比原生 C API)*],
      [↓],
      step[*静态解析引擎深度开发 (Lexer/Parser/语义分析)*],
      [↓],
      step[*论文撰写与性能二次优化 (目标：迁移至编译期映射)*],
    )
  ]
]

// ============================================
// 第五部分：进度安排
// ============================================
#make-section[进度安排]

#slide[
  = 进度安排

  #table(
    columns: (auto, 1fr, auto),
    inset: 12pt,
    align: left,
    stroke: none,
    fill: (x, y) => if y == 0 { rgb("#003366") } else if calc.odd(y) {
      rgb("#f5f5f5")
    } else { white },

    text(fill: white)[*阶段*],
    text(fill: white)[*主要任务*],
    text(fill: white)[*时间*],

    [第一阶段], [文献调研、技术路线论证、核心 DSL 语法定义], [第1-4周],
    [第二阶段], [运行时框架（事务、连接池）开发与编译期优化], [第5-8周],
    [第三阶段], [SQL 词法分析器（Lexer）与语法分析器开发], [第9-12周],
    [第四阶段], [静态语义校验引擎完善、多数据库适配与 Benchmark], [第13-15周],
    [第五阶段], [结果数据分析、论文撰写、答辩准备], [第16-18周],
  )
]

// ============================================
// 第六部分：已查阅的参考文献
// ============================================
#make-section[已查阅的参考文献]

#slide[
  = 已查阅的参考文献

  #set text(size: 14pt)
  #set list(spacing: 0.8em)

  [1] 邓际锋. 支持静态元编程的可扩展式开放编译器的研究与实现 [D]. 大连理工大学, 2006.

  [2] Abrahams D, Gurtovoy A. C++ Template Metaprogramming: Concepts, Tools, and Techniques from Boost and Beyond [M]. Addison-Wesley Professional, 2004.

  [3] Alexandrescu A. Modern C++ Design: Generic Programming and Design Patterns Applied [M]. Addison-Wesley, 2001.

  [4] Aho A V, Lam M S, Sethi R, et al. Compilers: Principles, Techniques, and Tools (2nd Edition) [M]. Pearson Education, 2006.

  [5] Vandevoorde D, Josuttis N M, Gregor D. C++ Templates: The Complete Guide (2nd Edition) [M]. Addison-Wesley Professional, 2017.

  [6] 徐绍铜. 基于 ORM 框架 Entity Framework 技术的研究 [J]. 电子技术与软件工程, 2016(18): 185-185.

  [7] 张仕, 毛宇光. XML 语法检查的实现 [J]. 计算机工程, 2002, 28(11).

  [8] mkitzan. constexpr-sql: A compile-time SQL library for modern C++ [EB/OL]. (2023-05-18) [2025-01-30]. https://github.com/mkitzan/constexpr-sql.

  [9] crabmandable. zxorm: A C++20 ORM library focusing on SQL generation and mapping [EB/OL]. (2024-03-20) [2025-01-30]. https://github.com/crabmandable/zxorm.
]

// ============================================
// 致谢页
// ============================================
#slide[
  #show: focus
  #set align(center + horizon) // 居中对齐，让视觉更集中

  #text(size: 2.5em, weight: "bold", fill: rgb("#003366"))[感谢各位老师指导！]

  #v(1em)

  #text(size: 1.2em, weight: "medium")[恳请各位老师批评指正]

  #v(0.5em)

  #text(size: 1em, fill: rgb("#333333"))[
    答辩人：*杨康*
  ]

  #text(size: 0.9em, fill: rgb("#666666"))[
    指导教师：*李辰*
  ]
]
