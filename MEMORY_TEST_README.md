# 内存泄漏检测配置说明

## 已完成的配置

### 1. CMake 配置
- 启用了 AddressSanitizer (`-fsanitize=address`)
- 启用了 LeakSanitizer (`-fsanitize=leak`)  
- 启用了 UndefinedBehaviorSanitizer (`-fsanitize=undefined`)
- 保留调试符号 (`-g`)
- 保留帧指针 (`-fno-omit-frame-pointer`)

### 2. 测试用例
在 `src/conn.cpp` 中添加了 15 个测试函数，覆盖：
- 内存泄漏
- 使用已释放内存
- 重复释放
- 缓冲区溢出
- 未初始化内存访问
- 空指针解引用
- 栈溢出
- 无效内存访问
- 内存对齐问题
- 资源泄漏
- ORM 特定内存管理
- 智能指针误用
- 线程安全问题
- 异常安全
- 字符串处理问题

### 3. 工具脚本
- `scripts/build_and_test.sh` - 构建并运行 ASan 测试
- `scripts/run_valgrind.sh` - 运行 Valgrind 内存分析

### 4. 问题清单
- `MEMORY_ISSUES_CHECKLIST.md` - 检测到的问题清单

## 使用方法

### 快速测试
```bash
./scripts/build_and_test.sh
```

### 详细分析
```bash
./scripts/run_valgrind.sh
```

### 手动构建和测试
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./orm
```

## 检测工具说明

### AddressSanitizer (ASan)
- 编译时插入检测代码
- 运行时检测内存错误
- 低性能开销
- 自动报告错误位置

### Valgrind
- 运行时二进制插桩
- 更详细的内存分析
- 性能开销较大
- 适合深度分析

## 预期输出

### ASan 检测到的问题
- 内存泄漏报告
- 堆栈使用错误
- 缓冲区溢出
- 使用已释放内存

### Valgrind 输出
- 详细的内存泄漏报告
- 非法内存访问
- 未初始化值使用
- 文件描述符泄漏

## 注意事项

1. **测试用例设计**: 部分危险操作已注释，避免程序崩溃
2. **数据库文件**: 确保 `./data/test.db` 存在或处理文件不存在的情况
3. **资源清理**: 测试中故意创建了一些资源泄漏
4. **修复顺序**: 建议按照清单优先级逐个修复

## 后续步骤

1. 运行检测脚本查看具体问题
2. 参考清单修复内存问题
3. 重新运行检测确认修复
4. 删除临时文件 (`MEMORY_ISSUES_CHECKLIST.md`, `MEMORY_TEST_README.md`)

---
*配置完成时间: 2025-02-24*