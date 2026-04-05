# ORM库迁移计划

## 声明

1. LLM完成任务后，请在任务前的方框内打勾，例如：[x]
2. **对代码的任何实质性更改都必须征求用户意见**，特别是：
   - API变更、头文件结构调整
   - 依赖项移除或替换方案
   - 构建系统重大修改
3. 每次完成重要更改后，需要告知用户具体更改内容
4. 本文件用于在上下文不足时，帮助新会话了解当前任务状态
5. **重要**：所有新编写的代码都需要更新到本文件"已编写代码"部分，形成记忆
6. **关键规则**：每次修改文件后，必须立即更新本文件中的相应任务状态和更改记录

## 项目概况

将 `orm_playground` 从应用程序重构为第三方库 `ess_orm`，要求：

- 同时提供静态库和动态库两种形式
- **完全隐藏** `tl::expected` 和 `fmt` 依赖（使用内部格式化器替代）
- 数据库驱动**优先使用系统版本**（sqlite3、mysql、postgres）
- 支持 **C++20 最高版本**
- 提供完整的**安装和打包支持**（CMake配置）

## 现有文件结构分析（记忆库）

### 公共头文件 (`include/ess/orm/`)

```
include/ess/orm/
├── common/                    # 通用组件
│   ├── concept.hpp           # C++20概念定义
│   ├── defines.hpp           # 平台宏（ESS_ORM_API等）
│   ├── error.hpp             # 错误处理（使用C++20 <format>）
│   └── meta.hpp              # 元编程工具（FixedString等）
├── config/                    # 配置系统
│   ├── config.hpp            # 配置入口（依赖fmt::print）
│   ├── default.hpp           # 默认配置
│   └── traits.hpp            # 配置特征萃取
├── core/                      # 核心ORM功能
│   ├── conn_factory.hpp      # 连接工厂接口
│   ├── connection.hpp        # 连接抽象接口
│   ├── connection_pool.hpp   # 连接池实现
│   ├── context.hpp           # 全局上下文单例
│   ├── defines.hpp           # 核心定义
│   ├── dialect.hpp           # 数据库方言定义
│   ├── impl.hpp              # 查询实现（核心逻辑）
│   ├── result.hpp            # 查询结果类型定义
│   ├── result_set_mapper.hpp # 结果集映射器
│   ├── row.hpp               # 动态行数据容器
│   ├── runtime.hpp           # 运行时查询接口（主要API）
│   ├── statement.hpp         # 语句抽象接口
│   └── transaction.hpp       # 事务处理
├── dsl/                       # 领域特定语言
│   ├── attribute.hpp         # 字段属性（依赖fmt::format）
│   ├── dsl.hpp               # DSL核心（依赖fmt::format/ranges）
│   └── traits.hpp            # 成员指针特征萃取
├── sql/                       # SQL处理
│   ├── common.hpp            # SQL AST通用定义
│   ├── lexer.hpp             # SQL词法分析器
│   ├── parser.hpp            # SQL语法分析器
│   ├── token.hpp             # 词法标记定义
│   └── validator.hpp         # SQL验证器
└── test/                      # 测试相关
    └── stress_test.hpp       # 压力测试定义
```

### 源代码文件 (`src/`)

```
src/
├── main.cpp                  # 示例程序（需要移出库）
├── schema.cpp                # 示例程序（需要移出库）
└── ess/orm/
    ├── core/
    │   ├── context.cpp       # 上下文实现
    │   └── sqlite3/          # SQLite3具体实现
    │       ├── sqlite3_conn_factory.cpp    # SQLite3连接工厂
    │       ├── sqlite3_connection.cpp      # SQLite3连接实现
    │       ├── sqlite3_connection.h        # SQLite3连接头文件
    │       ├── sqlite3_statement.cpp       # SQLite3语句实现
    │       └── sqlite3_statement.h         # SQLite3语句头文件
```

### 第三方依赖现状

1. **fmt**：硬性依赖，出现在：
   - `config/config.hpp:41` - `fmt::print`彩色输出
   - `dsl/attribute.hpp:4,231,237` - `fmt::format`属性字符串生成
   - `dsl/dsl.hpp:6,126` - `fmt::format`和`fmt::ranges`
   - `common/defines.hpp:27` - `ESS_FUNC_LOG`宏使用`fmt::println`

2. **tl::expected**：仅CMake链接，未在公共头文件中使用

3. **SQLite3**：实现依赖，通过`#include <sqlite3.h>`使用

## 迁移任务清单

### 阶段1：项目结构调整

[x] **1.1 创建库目标**：将`orm`可执行文件改为`ess_orm`库目标

- 修改CMakeLists.txt，创建`add_library(ess_orm ...)`
- 保持现有源代码结构，仅改变目标类型
- 需要用户确认：库命名是否合适？ ✅（用户确认：合适）

**更改记录**（2026-04-05）：
- 将 `add_executable(${exname} ...)` 改为 `add_library(ess_orm ...)`
- 将所有 `target_*` 命令中的目标名称从 `${exname}` 改为 `ess_orm`
- 保持源文件列表、包含目录、链接库等配置不变

[ ] **1.2 分离示例程序**：创建`examples/`目录

- 将`src/main.cpp`和`src/schema.cpp`移到`examples/`
- 创建`examples/CMakeLists.txt`，链接`ess_orm`库
- 需要用户确认：是否保留现有示例内容？

[ ] **1.3 重构CMake结构**：模块化构建系统

```
根目录CMakeLists.txt（主配置）
├── src/CMakeLists.txt（库构建）
├── examples/CMakeLists.txt（示例构建）
├── bench/CMakeLists.txt（性能测试）
└── tests/（未来单元测试）
```

### 阶段2：完全隐藏fmt依赖

[ ] **2.1 分析fmt使用场景**：

1. `config/config.hpp:41` - 彩色配置输出 → **可完全移除**（用户同意）
2. `dsl/attribute.hpp` - 属性字符串格式化 → 需要替换方案
3. `dsl/dsl.hpp` - DSL表达式格式化 → 需要替换方案
4. `common/defines.hpp:27` - 调试日志宏 → **可完全移除**（用户同意）

[ ] **2.2 实现简单内部格式化器**（方案B）：

- 创建`include/ess/orm/internal/string_utils.hpp`
- 功能：基本字符串拼接，支持`int`、`float`、`enum`、字符串字面量
- 接口示例：
  ```cpp
  namespace ess::orm::internal {
    template<typename... Args>
    std::string concat(Args&&... args);  // 简单拼接

    template<typename T>
    std::string to_string(T value);      // 类型到字符串转换
  }
  ```

[ ] **2.3 替换config/config.hpp中的fmt::print**：

- 移除`print_config()`函数或改为使用标准输出
- 需要用户确认：是否完全移除配置打印功能？

[ ] **2.4 替换dsl/attribute.hpp中的fmt::format**：

- `format_attribute()`函数改为使用内部格式化器
- 注意处理浮点数、枚举等特殊类型
- 复用现有逻辑，仅替换格式化后端

[ ] **2.5 替换dsl/dsl.hpp中的fmt::format和fmt::ranges**：

- `format_fields()`函数改为使用内部格式化器
- 实现简单的范围拼接功能
- 需要用户确认：DSL格式化要求（仅需基本拼接即可）

[ ] **2.6 移除common/defines.hpp中的ESS_FUNC_LOG宏**：

- 直接删除或提供无依赖替代方案
- 需要用户确认：是否保留调试日志功能？

[ ] **2.7 验证公共头文件无fmt依赖**：

- 检查所有公共头文件不再包含`<fmt/*>`头文件
- 确保`ess_orm`库不公开链接`fmt`

### 阶段3：数据库驱动配置（优先系统版本）

[ ] **3.1 SQLite3系统库支持**：

- 添加CMake选项：`USE_SYSTEM_SQLITE3`（默认`ON`）
- 实现查找逻辑：
  ```cmake
  if(USE_SYSTEM_SQLITE3)
    find_package(SQLite3 REQUIRED)
    target_link_libraries(ess_orm PRIVATE SQLite3::SQLite3)
    target_include_directories(ess_orm SYSTEM PRIVATE ${SQLite3_INCLUDE_DIRS})
  else()
    add_external_subproject("thirdparties/sqlite3" "SQLite3")
    target_link_libraries(ess_orm PRIVATE SQLite3)
  endif()
  ```

[ ] **3.2 SQLite3头文件隔离**：

- 确保`sqlite3.h`不通过公共API暴露
- 检查`sqlite3_connection.h`和`sqlite3_statement.h`是否为私有头文件
- 需要用户确认：是否将SQLite3实现头文件移出公共目录？

[ ] **3.3 预留MySQL/PostgreSQL支持框架**：

- 在`dialect.hpp`中定义方言枚举
- 在CMake中添加查找系统库的占位逻辑
- 创建对应的连接工厂和实现框架

### 阶段4：构建类型支持（静态库+动态库）

[ ] **4.1 配置BUILD_SHARED_LIBS选项**：

- 在根CMakeLists.txt中添加`option(BUILD_SHARED_LIBS "Build shared library" ON)`
- 根据选项设置`ESS_ORM_EXPORTS`宏

[ ] **4.2 完善符号导出宏**：

- 更新`common/defines.hpp`中的`ESS_ORM_API`宏
- 确保动态库导出所有必要符号
- 静态库时宏为空

[ ] **4.3 测试两种构建类型**：

- 分别构建静态库和动态库
- 验证示例程序能正常链接和使用

### 阶段5：安装与打包支持

[ ] **5.1 添加安装规则**：

```cmake
install(TARGETS ess_orm
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ess/orm
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ess
  FILES_MATCHING PATTERN "*.hpp"
)
```

[ ] **5.2 生成CMake配置文件**：

- 创建`ess_ormConfig.cmake.in`模板
- 生成`ess_ormConfig.cmake`和`ess_ormConfigVersion.cmake`
- 正确导出目标，隐藏私有依赖（tl::expected）

[ ] **5.3 配置版本信息**：

- 在CMake中定义版本号（当前0.0.1）
- 生成版本头文件`include/ess/orm/version.hpp`

[ ] **5.4 支持包管理器**：

- 生成`pkg-config`文件（`.pc`）
- 考虑未来支持vcpkg/Conan

### 阶段6：代码复用与优化

[ ] **6.1 检查可复用代码**：

- `meta.hpp`中的`FixedString`、类型标签系统 → **完全复用**
- `concept.hpp`中的概念定义 → **完全复用**
- `error.hpp`中的错误处理（使用C++20 `<format>`） → **完全复用**
- SQL词法/语法分析器 → **完全复用**

[ ] **6.2 优化头文件包含关系**：

- 创建`include/ess/orm/orm.hpp`主包含文件
- 减少头文件间不必要的依赖
- 使用前向声明减少编译时间

[ ] **6.3 移除不必要代码**：

- 检查`test/stress_test.hpp`是否应为公共API → 建议移出
- 清理注释掉的代码和调试输出

### 阶段7：文档与示例更新

[ ] **7.1 更新README.md**：

- 说明库的构建、安装和使用方法
- 列出配置选项和依赖要求
- 提供简单使用示例

[ ] **7.2 创建使用示例**：

- 在`examples/`目录中添加更多示例
- 展示基本CRUD操作、事务使用
- 演示多数据库配置（预留）

## 实施优先级和时间估计

### 高优先级（必须先完成）

1. **阶段1**：项目结构调整（1天）
   - 基础框架，后续工作的前提

2. **阶段2.1-2.3**：fmt依赖分析和基本移除（1天）
   - 移除简单的fmt使用（配置打印、调试日志）

3. **阶段4**：构建类型支持（0.5天）
   - 库类型支持，影响用户使用方式

### 中优先级（核心功能）

4. **阶段2.4-2.7**：DSL格式化器替换（2-3天）
   - 工作量最大，需要小心处理
   - 必须确保功能完整性和性能

5. **阶段3**：数据库驱动配置（1天）
   - 系统库优先，影响部署

6. **阶段5**：安装打包（1天）
   - 分发支持，影响其他项目集成

### 低优先级（完善工作）

7. **阶段6**：代码优化（1天）
   - 提高代码质量和可维护性

8. **阶段7**：文档更新（0.5天）
   - 用户指导

**总估计时间**：7-8天

## 关键决策点（需用户确认）

### 1. fmt替换策略

- **方案B**：简单内部格式化器 ✅（用户已选择）
  - 仅支持基本字符串拼接
  - 不支持复杂格式化（如宽度、精度、填充）
  - 性能要求：尽可能高效

### 2. 配置打印功能

- 完全移除`print_config()`？ ✅（用户同意移除彩色输出）
- 替换为简单标准输出？
- 提供可选日志回调接口？

### 3. 调试日志

- 完全移除`ESS_FUNC_LOG`宏？ ✅（用户未要求保留）
- 提供无依赖的简化版本？

### 4. SQLite3实现头文件位置

- 当前：`src/ess/orm/core/sqlite3/*.h`
- 建议：保持原位置（私有头文件）
- 确保不通过公共API暴露

### 5. 库命名

- 主库：`ess_orm`
- 命名空间：`ess::orm`
- 需要用户确认：命名是否合适？

### 6. 版本号

- 当前：0.0.1（实验性）
- 迁移后建议：0.1.0（第一个库版本）
- 需要用户确认：版本号策略？

## 风险与缓解措施

### 高风险：DSL格式化器替换

- **风险**：格式化逻辑复杂，可能引入错误
- **缓解**：
  1. 分步骤替换，每个文件单独验证
  2. 保持单元测试（如有）
  3. 手动测试示例程序

### 中风险：动态库符号导出

- **风险**：Windows平台动态库链接问题
- **缓解**：
  1. 仔细设计`ESS_ORM_API`宏
  2. 测试多平台构建
  3. 提供清晰的错误信息

### 低风险：系统库查找

- **风险**：不同系统包管理器路径差异
- **缓解**：
  1. 使用CMake标准`find_package`
  2. 提供详细的错误提示
  3. 文档说明依赖安装方法

## 验证计划

### 构建验证

1. 静态库构建：`cmake -DBUILD_SHARED_LIBS=OFF ..`
2. 动态库构建：`cmake -DBUILD_SHARED_LIBS=ON ..`
3. 系统SQLite3：`cmake -DUSE_SYSTEM_SQLITE3=ON ..`
4. 捆绑SQLite3：`cmake -DUSE_SYSTEM_SQLITE3=OFF ..`

### 功能验证

1. 示例程序编译运行
2. 基本CRUD操作测试
3. 事务功能测试
4. 多线程连接池测试

### 依赖验证

1. 检查生成的库文件依赖项（`ldd`/`otool`）
2. 验证公共头文件无外部依赖
3. 检查安装包内容完整性

## 补充内容

### 已编写代码（记忆更新）

#### 2026-04-05：阶段1.1 - CMakeLists.txt 修改
**文件**: `/Users/wuming/code/orm_playground/CMakeLists.txt`
**更改内容**:
- 将 `add_executable(${exname} ...)` 改为 `add_library(ess_orm ...)`
- 将所有 `target_*` 命令中的目标名称从 `${exname}` 改为 `ess_orm`
- 保持源文件列表、包含目录、链接库等配置不变

**效果**: 构建目标从可执行文件 `orm` 改为库文件 `ess_orm`

### 任务依赖关系

1. 阶段1必须在所有其他阶段之前完成
2. 阶段2.1-2.3完成后才能进行阶段2.4-2.7
3. 阶段4（构建类型）应在阶段1后尽早完成，便于后续测试
4. 阶段5（安装打包）依赖阶段1-4完成

### 具体实施步骤补充

#### 阶段1.1详细步骤：

1. 备份当前CMakeLists.txt
2. 修改`add_executable`为`add_library`
3. 调整目标链接依赖
4. 创建库导出宏配置

#### 阶段2.2详细步骤：

1. 创建`include/ess/orm/internal/`目录
2. 实现`to_string`模板特化：
   - 整数类型：`std::to_string`
   - 浮点类型：`std::to_string`或自定义格式化
   - 枚举类型：转换为底层整数
   - 字符串类型：直接返回
   - 编译期字符串：转换为运行时字符串

### 检查点列表

- [ ] 阶段1完成后：库能成功构建为`ess_orm`
- [ ] 阶段2.1-2.3完成后：`config/config.hpp`和`common/defines.hpp`无fmt依赖
- [ ] 阶段2.4-2.7完成后：所有公共头文件无fmt依赖
- [ ] 阶段3完成后：CMake选项`USE_SYSTEM_SQLITE3`正常工作
- [ ] 阶段4完成后：静态库和动态库都能构建
- [ ] 阶段5完成后：`make install`正常工作

### 时间追踪

- **开始日期**：2026-04-05
- **预计完成日期**：2026-04-12
- **实际进度**：5%（阶段1.1完成）

## 后续工作（迁移完成后）

### 测试开发

- 添加单元测试框架
- 创建集成测试套件
- 性能基准测试

### 功能扩展

- MySQL/PostgreSQL后端实现
- 异步查询支持（C++20协程）
- 高级ORM功能（关联关系、迁移工具）

### 生态建设

- 包管理器支持（vcpkg、Conan）
- 文档网站生成
- 示例项目库

---

**最后更新**：2026-04-05  
**当前状态**：计划制定完成，等待执行
