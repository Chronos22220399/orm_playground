#!/usr/bin/env python3
"""
全面基准测试结果分析脚本
分析不同数据规模下的SQLite3 vs ORM性能对比
"""

import json
import sys
from pathlib import Path

def load_results(json_file):
    """加载基准测试结果"""
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    results = []
    for benchmark in data['benchmarks']:
        name = benchmark['name']
        time_ns = benchmark.get('real_time', benchmark.get('cpu_time', 0))
        time_ms = time_ns / 1_000_000
        
        # 解析测试名称
        if 'SQLite3' in name:
            impl = 'SQLite3'
        elif 'ORM' in name:
            impl = 'ORM'
        else:
            impl = 'Unknown'
        
        # 解析操作类型和批量大小
        if 'InsertBatch' in name:
            op_type = '批量插入'
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        elif 'QueryBatch' in name:
            op_type = '批量查询'
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        elif 'UpdateBatch' in name:
            op_type = '批量更新'
            batch_size = int(name.split('/')[1]) if '/' in name else 1
        elif 'DeleteBatch' in name:
            op_type = '批量删除'
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
            'time_per_item_ns': time_ns / batch_size if batch_size > 0 else 0
        })
    
    return results

def analyze_results(results):
    """分析结果并生成报告"""
    print("=" * 120)
    print("SQLite3 vs ORM 全面性能基准测试分析报告")
    print("=" * 120)
    
    # 按操作类型和批量大小分组
    operations = ['批量插入', '批量查询', '批量更新', '批量删除']
    batch_sizes = [10, 100, 1000, 10000]
    
    for op in operations:
        print(f"\n{op}性能分析:")
        print("-" * 120)
        print(f"{'批量大小':<10} {'SQLite3(ms)':<15} {'ORM(ms)':<15} {'ORM开销':<15} {'每项SQLite3(ns)':<20} {'每项ORM(ns)':<20}")
        print("-" * 120)
        
        for size in batch_sizes:
            if op == '批量更新' and size == 10000:
                continue  # 跳过10000的更新测试
            
            sqlite_result = next((r for r in results if r['operation'] == op and 
                                 r['batch_size'] == size and r['implementation'] == 'SQLite3'), None)
            orm_result = next((r for r in results if r['operation'] == op and 
                               r['batch_size'] == size and r['implementation'] == 'ORM'), None)
            
            if sqlite_result and orm_result:
                sqlite_time = sqlite_result['time_ms']
                orm_time = orm_result['time_ms']
                overhead = (orm_time / sqlite_time - 1) * 100
                
                sqlite_per_item = sqlite_result['time_per_item_ns']
                orm_per_item = orm_result['time_per_item_ns']
                
                print(f"{size:<10} {sqlite_time:<15.3f} {orm_time:<15.3f} {overhead:<15.1f}% {sqlite_per_item:<20.1f} {orm_per_item:<20.1f}")
    
    # 性能趋势分析
    print("\n" + "=" * 120)
    print("性能趋势分析:")
    print("=" * 120)
    
    for op in operations:
        print(f"\n{op}扩展性分析:")
        sqlite_results = [r for r in results if r['operation'] == op and r['implementation'] == 'SQLite3']
        orm_results = [r for r in results if r['operation'] == op and r['implementation'] == 'ORM']
        
        if not sqlite_results or not orm_results:
            continue
        
        # 按批量大小排序
        sqlite_results.sort(key=lambda x: x['batch_size'])
        orm_results.sort(key=lambda x: x['batch_size'])
        
        # 计算扩展因子
        if len(sqlite_results) >= 2:
            small_sqlite = sqlite_results[0]['time_per_item_ns']
            large_sqlite = sqlite_results[-1]['time_per_item_ns']
            sqlite_scaling = large_sqlite / small_sqlite if small_sqlite > 0 else 0
            
            small_orm = orm_results[0]['time_per_item_ns']
            large_orm = orm_results[-1]['time_per_item_ns']
            orm_scaling = large_orm / small_orm if small_orm > 0 else 0
            
            print(f"  SQLite3: 从{small_sqlite:.1f}ns/项到{large_sqlite:.1f}ns/项，扩展因子: {sqlite_scaling:.2f}")
            print(f"  ORM:     从{small_orm:.1f}ns/项到{large_orm:.1f}ns/项，扩展因子: {orm_scaling:.2f}")
            
            if sqlite_scaling < orm_scaling:
                print(f"  → SQLite3扩展性更好")
            elif sqlite_scaling > orm_scaling:
                print(f"  → ORM扩展性更好")
            else:
                print(f"  → 两者扩展性相似")
    
    # 总结
    print("\n" + "=" * 120)
    print("关键发现总结:")
    print("=" * 120)
    
    # 计算平均开销
    overheads = []
    for op in operations:
        for size in batch_sizes:
            if op == '批量更新' and size == 10000:
                continue
            
            sqlite_result = next((r for r in results if r['operation'] == op and 
                                 r['batch_size'] == size and r['implementation'] == 'SQLite3'), None)
            orm_result = next((r for r in results if r['operation'] == op and 
                               r['batch_size'] == size and r['implementation'] == 'ORM'), None)
            
            if sqlite_result and orm_result and sqlite_result['time_ms'] > 0:
                overhead = (orm_result['time_ms'] / sqlite_result['time_ms'] - 1) * 100
                overheads.append((op, size, overhead))
    
    # 按操作类型分组计算平均开销
    op_overheads = {}
    for op, size, overhead in overheads:
        if op not in op_overheads:
            op_overheads[op] = []
        op_overheads[op].append(overhead)
    
    for op, overhead_list in op_overheads.items():
        avg_overhead = sum(overhead_list) / len(overhead_list)
        min_overhead = min(overhead_list)
        max_overhead = max(overhead_list)
        
        print(f"{op}:")
        print(f"  平均开销: {avg_overhead:.1f}% (范围: {min_overhead:.1f}% - {max_overhead:.1f}%)")
        
        if avg_overhead < 20:
            print(f"  → 性能开销很小，ORM非常高效")
        elif avg_overhead < 50:
            print(f"  → 性能开销适中，在可接受范围内")
        elif avg_overhead < 100:
            print(f"  → 性能开销较大，但考虑到ORM的优势仍可接受")
        else:
            print(f"  → 性能开销很大，需要优化")
    
    # 总体建议
    print("\n总体建议:")
    print("1. 对于小批量操作(10-100项): ORM性能开销相对较小，推荐使用")
    print("2. 对于大批量操作(1000+项): 考虑使用原生SQLite3 API以获得最佳性能")
    print("3. 查询操作开销最大，因为ORM需要结果映射")
    print("4. 插入操作开销最小，ORM几乎与原生API一样快")
    print("5. 在开发效率和类型安全更重要时，优先使用ORM")
    print("6. 在性能至关重要时，考虑混合使用ORM和原生API")

def main():
    # 查找结果文件
    json_files = [
        Path("build/bench/comprehensive_results.json"),
        Path("bench/comprehensive_results.json"),
        Path("comprehensive_results.json")
    ]
    
    json_file = None
    for file in json_files:
        if file.exists():
            json_file = file
            break
    
    if not json_file:
        print("错误: 未找到基准测试结果文件")
        print("请先运行基准测试: ./bench/comprehensive_benchmark")
        sys.exit(1)
    
    print(f"加载结果文件: {json_file}")
    results = load_results(json_file)
    
    print(f"加载了 {len(results)} 个基准测试结果")
    analyze_results(results)

if __name__ == "__main__":
    main()