#!/usr/bin/env python3
"""
SQLite3 vs ORM 基准测试结果简单可视化脚本
不需要外部依赖，只使用标准库
"""

import json
import sys
from pathlib import Path

def load_benchmark_results(json_file):
    """加载基准测试结果"""
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    results = []
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        time_ns = benchmark.get('real_time', benchmark.get('cpu_time', 0))
        time_ms = time_ns / 1_000_000  # 转换为毫秒
        
        # 解析测试名称
        if 'SQLite3' in name:
            impl = 'SQLite3'
        elif 'ORM' in name:
            impl = 'ORM'
        else:
            impl = 'Unknown'
        
        # 解析操作类型
        if 'InsertSingle' in name:
            op_type = '插入单个'
            batch_size = 1
        elif 'InsertBatch' in name:
            op_type = '批量插入'
            # 从参数中提取批量大小
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        elif 'QuerySingle' in name:
            op_type = '查询单个'
            batch_size = 1
        elif 'QueryMultiple' in name:
            op_type = '查询多个'
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        elif 'Update' in name:
            op_type = '更新'
            batch_size = 1
        elif 'Delete' in name:
            op_type = '删除'
            batch_size = 1
        elif 'Transaction' in name:
            op_type = '事务'
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        else:
            op_type = '其他'
            batch_size = 1
        
        results.append({
            'name': name,
            'implementation': impl,
            'operation': op_type,
            'batch_size': batch_size,
            'time_ns': time_ns,
            'time_ms': time_ms,
            'iterations': benchmark.get('iterations', 1)
        })
    
    return results

def print_results_table(results):
    """打印结果表格"""
    print("=" * 100)
    print("SQLite3 vs ORM 性能基准测试结果")
    print("=" * 100)
    print(f"{'操作类型':<15} {'实现':<10} {'批量大小':<10} {'时间(ms)':<15} {'相对性能':<15}")
    print("-" * 100)
    
    # 按操作类型和批量大小分组
    grouped = {}
    for result in results:
        key = (result['operation'], result['batch_size'])
        if key not in grouped:
            grouped[key] = {'SQLite3': None, 'ORM': None}
        grouped[key][result['implementation']] = result['time_ms']
    
    # 打印每个操作类型的结果
    for (op_type, batch_size), times in sorted(grouped.items()):
        sqlite_time = times.get('SQLite3')
        orm_time = times.get('ORM')
        
        if sqlite_time and orm_time:
            # 计算相对性能（ORM相对于SQLite3）
            if sqlite_time > 0:
                relative = orm_time / sqlite_time
                perf_text = f"ORM是SQLite3的{relative:.2f}倍"
                if relative < 1:
                    perf_text = f"ORM比SQLite3快{(1/relative):.1f}倍"
                else:
                    perf_text = f"SQLite3比ORM快{relative:.1f}倍"
            else:
                perf_text = "N/A"
            
            batch_display = f"{batch_size}" if batch_size > 1 else "1"
            print(f"{op_type:<15} {'SQLite3':<10} {batch_display:<10} {sqlite_time:<15.3f} {'基准':<15}")
            print(f"{op_type:<15} {'ORM':<10} {batch_display:<10} {orm_time:<15.3f} {perf_text:<15}")
            print("-" * 100)
    
    print("\n关键发现:")
    print("1. 单个操作性能:")
    single_ops = [r for r in results if r['batch_size'] == 1]
    for op in ['插入单个', '查询单个', '更新', '删除']:
        sqlite = next((r for r in single_ops if r['operation'] == op and r['implementation'] == 'SQLite3'), None)
        orm = next((r for r in single_ops if r['operation'] == op and r['implementation'] == 'ORM'), None)
        if sqlite and orm:
            ratio = orm['time_ms'] / sqlite['time_ms']
            if ratio < 1:
                print(f"   - {op}: ORM比SQLite3快{(1/ratio):.1f}倍")
            else:
                print(f"   - {op}: SQLite3比ORM快{ratio:.1f}倍")
    
    print("\n2. 批量操作扩展性:")
    batch_sizes = sorted(set(r['batch_size'] for r in results if r['batch_size'] > 1))
    for size in batch_sizes:
        batch_ops = [r for r in results if r['batch_size'] == size]
        sqlite_avg = sum(r['time_ms'] for r in batch_ops if r['implementation'] == 'SQLite3') / len([r for r in batch_ops if r['implementation'] == 'SQLite3'])
        orm_avg = sum(r['time_ms'] for r in batch_ops if r['implementation'] == 'ORM') / len([r for r in batch_ops if r['implementation'] == 'ORM'])
        print(f"   - 批量大小 {size}: SQLite3={sqlite_avg:.1f}ms, ORM={orm_avg:.1f}ms")

def main():
    # 查找结果文件
    json_files = [
        Path("bench/results.json"),
        Path("build/bench/results.json"),
        Path("results.json")
    ]
    
    json_file = None
    for file in json_files:
        if file.exists():
            json_file = file
            break
    
    if not json_file:
        print("错误: 未找到基准测试结果文件")
        print("请先运行基准测试: ./bench/run_benchmark.sh")
        sys.exit(1)
    
    print(f"加载结果文件: {json_file}")
    results = load_benchmark_results(json_file)
    
    print(f"加载了 {len(results)} 个基准测试结果")
    print_results_table(results)

if __name__ == "__main__":
    main()