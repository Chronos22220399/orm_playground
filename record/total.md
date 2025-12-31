## 命名规范

1. 数据/属性类：CamelCase
2. 普通：snake_case
3. 静态: snake_case
4. 函数: snake_case
5. 概念: snake_case
6. 检测用途的类: snake_case
7. 宏: ALL_CAPS

## 总体 schema

```C++
namespace ess {
// 存储元类型
namespace meta {

}

namespace orm {

// 用户接口
namespace dsl {}

// 属性定义
namespace attribute {}

// 解释器
namespace interpret {}

// 实现细节
namespace detail {}

// 运行时工具
namespace runtime {}

// 缓存
namespace cache {}
}

}
```

## 各部分作用

1. schema 层仅做声明
