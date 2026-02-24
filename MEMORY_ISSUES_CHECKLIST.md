# 内存泄漏和错误检测清单

## 测试配置
- ✅ AddressSanitizer (ASan) 已启用
- ✅ LeakSanitizer 已启用
- ✅ UndefinedBehaviorSanitizer 已启用
- ✅ Valgrind 测试脚本已创建

## 检测到的问题类型

### 1. 内存泄漏 (Memory Leaks)
- [ ] `test_memory_leak()`: `new int[100]` 未释放
- [ ] `test_resource_leak()`: 文件句柄未关闭 (`fopen` 未 `fclose`)

### 2. 使用已释放内存 (Use After Free)
- [ ] `test_use_after_free()`: 删除指针后继续使用
- [ ] `test_smart_pointer_misuse()`: 从 shared_ptr 获取的原始指针可能在使用后失效

### 3. 重复释放 (Double Free)
- [ ] `test_double_free()`: 同一内存释放两次

### 4. 缓冲区溢出 (Buffer Overflow)
- [ ] `test_buffer_overflow()`: 潜在缓冲区溢出风险
- [ ] `test_string_handling()`: 字符串复制可能溢出小缓冲区

### 5. 未初始化内存访问 (Uninitialized Memory Access)
- [ ] `test_uninitialized_memory()`: 读取未初始化变量

### 6. 空指针解引用 (Null Pointer Dereference)
- [ ] `test_null_pointer_dereference()`: 解引用空指针

### 7. 栈溢出 (Stack Overflow)
- [ ] `test_stack_overflow()`: 深度递归可能导致栈溢出

### 8. 无效内存访问 (Invalid Memory Access)
- [ ] `test_invalid_memory_access()`: 访问无效内存地址

### 9. 内存对齐问题 (Memory Alignment)
- [ ] `test_memory_alignment()`: 未对齐的内存访问

### 10. 循环引用 (Circular References)
- [ ] `test_smart_pointer_misuse()`: shared_ptr 循环引用可能导致内存泄漏

### 11. 线程安全问题 (Thread Safety)
- [ ] `test_thread_safety()`: 非原子操作共享变量

### 12. 异常安全 (Exception Safety)
- [ ] `test_exception_safety()`: 异常抛出时资源可能泄漏

### 13. ORM 特定问题
- [ ] 连接管理: 多个连接创建和销毁
- [ ] 语句处理: 准备语句的内存管理
- [ ] 事务处理: 开始/提交/回滚的资源管理
- [ ] 结果集迭代: 迭代过程中的内存使用

## 运行检测

### 使用 AddressSanitizer
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./orm
```

### 使用 Valgrind
```bash
./scripts/run_valgrind.sh
```

### 预期输出检查
1. ASan 会报告内存泄漏和错误
2. Valgrind 会提供详细的内存使用报告
3. 检查 `valgrind_output.txt` 文件中的错误摘要

## 修复建议

### 高优先级
1. 确保所有 `new`/`malloc` 都有对应的 `delete`/`free`
2. 使用智能指针 (`std::unique_ptr`, `std::shared_ptr`) 管理资源
3. 实现 RAII 模式管理文件句柄、数据库连接等资源

### 中优先级
1. 添加边界检查防止缓冲区溢出
2. 初始化所有变量
3. 检查指针有效性后再使用

### 低优先级
1. 优化内存对齐
2. 添加线程安全保护
3. 完善异常安全保证

## 注意事项
1. 此文件为临时检测清单，修复问题后可删除
2. 实际修复时需考虑代码的业务逻辑
3. 建议逐个问题验证和修复
4. 修复后重新运行检测工具确认问题已解决

---
*生成时间: 2025-02-24*
*工具: AddressSanitizer, Valgrind*
*测试文件: src/conn.cpp*