# SQL Parser 代码结构说明

## 重要原则

**绝对不能删除任何文件！**
- 所有代码文件一旦创建，禁止删除
- 如需修改，只能添加或替换，禁止删除
- 删除操作必须获得用户明确授权

## 目录结构

```
include/ess/orm/sql/
├── parser.hpp          # 主入口，包含 parser_decl.hpp
├── parser_decl.hpp    # Parser 类声明（从 parser.hpp 分离出来）
├── token.hpp        # Token 定义
├── lexer.hpp      # Lexer 定义
├── sql_error_kind.hpp  # 错误类型定义
├── validator.hpp  # SQL 验证器
└── details/       # Parser 实现细节（**禁止删除目录**）
    ├── parser_common.ipp   # 通用解析函数实现
    └── parser_select.ipp # SELECT 解析函数实现
```

## 调用关系

1. **用户代码** 包含 `<ess/orm/orm.hpp>`
2. **orm.hpp** 包含 `<ess/orm/sql/parser.hpp>`
3. **parser.hpp** 引入：
   - `parser_decl.hpp` - 类声明
   - `details/parser_common.ipp` - 通用解析实现
   - `details/parser_select.ipp` - SELECT 解析实现
4. **parser_decl.hpp** 定义 `Parser` 类声明（仅声明函数，不包含实现）

## 文件依赖

- `parser.hpp` 依赖 `parser_decl.hpp`
- `parser_common.ipp` 依赖 `parser_decl.hpp`
- `parser_select.ipp` 依赖 `parser_decl.hpp`

## Git 操作限制

- **只读权限**：只能查看，不能变更
- 任何 `git rm`, `git mv`, `git checkout --`, `git restore` 等删除操作需要用户授权
- 禁止使用 `git clean` 命令
- 删除文件必须提交申请

## 新文件添加流程

1. 在 `details/` 目录创建新 `.ipp` 文件
2. 在 `parser.hpp` 中添加 `#include`
3. 更新本文档说明新文件用途