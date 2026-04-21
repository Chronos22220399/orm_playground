#!/bin/zsh

# ===== 配置 =====
BUILD_DIR="build"
CPPINSIGHTS="/opt/homebrew/Cellar/cppinsights/20.1_1/bin/insights"
# ===== 解析参数 =====
case "$1" in
    # 模板调试模式
    debug|d)
        echo "🔍 模板调试模式"
        cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
              -DCMAKE_CXX_STANDARD=20 \
              -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
              -B ${BUILD_DIR}
        
        # 展开指定文件（默认 examples/schema.cpp）
        FILE="${2:-examples/schema.cpp}"
        ${CPPINSIGHTS} ${FILE} -p ${BUILD_DIR}/compile_commands.json > expanded.cpp
        echo "✅ 已生成 expanded.cpp"
        
        # 可选：用编辑器打开
        # code expanded.cpp
        exit 0
        ;;
    
    # 清理
    clean|c)
        rm -rf ${BUILD_DIR} expanded.cpp
        echo "✅ 已清理"
        exit 0
        ;;
    
    # 仅编译
    build)
        cmake --build ${BUILD_DIR} -j 4
        exit $?
        ;;

    # 运行特定示例
    example|e)
        EXAMPLE="${2:-schema}"
        if [ "$EXAMPLE" = "schema" ]; then
            ./${BUILD_DIR}/examples/schema_example
        elif [ "$EXAMPLE" = "orm" ]; then
            ./${BUILD_DIR}/examples/orm_example
        else
            echo "❌ 未知示例: $EXAMPLE"
            echo "可用示例: schema, orm"
            exit 1
        fi
        exit $?
        ;;

    # 运行基准测试
    benchmark|bench|b)
        BENCH_TYPE="${2:-all}"
        
        # 确保构建目录存在
        if [ ! -d "${BUILD_DIR}" ]; then
            echo "📦 构建目录不存在，正在构建..."
            cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
                  -DCMAKE_CXX_STANDARD=20 \
                  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
                  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                  -B ${BUILD_DIR}
            cmake --build ${BUILD_DIR} -j 4
        fi
        
        case "$BENCH_TYPE" in
            all)
                echo "📊 运行所有基准测试..."
                echo "=========================================="
                echo "1. 基础基准测试 (原始版本)"
                ./${BUILD_DIR}/bench/sqlite_vs_orm_benchmark
                echo ""
                echo "2. 公平基准测试 (优化对比)"
                ./${BUILD_DIR}/bench/fair_benchmark
                echo ""
                echo "3. 全面基准测试 (不同数据规模)"
                ./${BUILD_DIR}/bench/comprehensive_benchmark --benchmark_out=${BUILD_DIR}/bench/comprehensive_results.json --benchmark_out_format=json
                echo ""
                echo "4. 文件数据库基准测试"
                ./${BUILD_DIR}/bench/file_benchmark --benchmark_out=${BUILD_DIR}/bench/file_results.json --benchmark_out_format=json
                echo ""
                echo "✅ 所有基准测试完成"
                echo "📈 运行分析脚本查看结果:"
                echo "   python3 bench/analyze_comprehensive.py"
                echo "   python3 bench/analyze_file_results.py"
                ;;
            basic)
                echo "📊 运行基础基准测试..."
                ./${BUILD_DIR}/bench/sqlite_vs_orm_benchmark
                ;;
            fair)
                echo "📊 运行公平基准测试..."
                ./${BUILD_DIR}/bench/fair_benchmark
                ;;
            comprehensive|comp)
                echo "📊 运行全面基准测试..."
                ./${BUILD_DIR}/bench/comprehensive_benchmark --benchmark_out=${BUILD_DIR}/bench/comprehensive_results.json --benchmark_out_format=json
                echo "📈 运行分析脚本查看结果:"
                echo "   python3 bench/analyze_comprehensive.py"
                ;;
            file)
                echo "📊 运行文件数据库基准测试..."
                ./${BUILD_DIR}/bench/file_benchmark --benchmark_out=${BUILD_DIR}/bench/file_results.json --benchmark_out_format=json
                echo "📈 运行分析脚本查看结果:"
                echo "   python3 bench/analyze_file_results.py"
                ;;
            analyze|a)
                echo "📈 分析基准测试结果..."
                if [ -f "${BUILD_DIR}/bench/comprehensive_results.json" ]; then
                    echo "分析全面基准测试结果:"
                    python3 bench/analyze_comprehensive.py
                    echo ""
                fi
                if [ -f "${BUILD_DIR}/bench/file_results.json" ]; then
                    echo "分析文件数据库基准测试结果:"
                    python3 bench/analyze_file_results.py
                fi
                ;;
            chart|c)
                echo "📊 生成图表可视化..."
                if [ ! -f "venv/bin/activate" ]; then
                    echo "创建Python虚拟环境..."
                    python3 -m venv venv
                fi
                source venv/bin/activate
                python3 bench/chart_visualization.py
                echo ""
                echo "📈 图表已生成到: bench/charts/"
                echo "📄 生成HTML报告..."
                echo "✅ HTML报告已生成: bench/benchmark_report.html"
                echo "使用以下命令查看:"
                echo "  open bench/benchmark_report.html  # 在浏览器中打开报告"
                echo "  open bench/charts/  # 在Finder中打开图表目录"
                ;;
            *)
                echo "❌ 未知基准测试类型: $BENCH_TYPE"
                echo "可用类型:"
                echo "  all           所有基准测试"
                echo "  basic         基础基准测试 (原始版本)"
                echo "  fair          公平基准测试 (优化对比)"
                echo "  comprehensive 全面基准测试 (不同数据规模)"
                echo "  file          文件数据库基准测试"
                echo "  analyze       分析已有结果"
                echo "  chart         生成图表可视化"
                exit 1
                ;;
        esac
        exit $?
        ;;

    # 帮助
    help|h)
        echo "用法: $0 [命令] [参数]"
        echo "  (无参数)  完整构建并运行两个示例"
        echo "  debug|d   展开模板到 expanded.cpp"
        echo "  clean|c   清理构建目录"
        echo "  build     仅编译"
        echo "  example|e [schema|orm]  运行特定示例"
    echo "  benchmark|bench|b [类型]  运行基准测试"
    echo "    类型: all, basic, fair, comprehensive, file, analyze, chart"
        echo "  help|h    显示帮助"
        exit 0
        ;;
esac
# ===== 默认：完整构建流程 =====
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
      -DCMAKE_CXX_STANDARD=20 \
      -DCMAKE_CXX_STANDARD_REQUIRED=ON \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -B ${BUILD_DIR}
cmake --build ${BUILD_DIR} -j 4
if [[ $? -ne 0 ]]; then
    echo "\n❌ 编译失败"
    echo "💡 提示: 运行 '$0 debug' 查看模板展开"
    exit 1
fi
# 运行两个示例
echo "🚀 运行 orm_example..."
./${BUILD_DIR}/examples/orm_example
# echo ""
# echo "🚀 运行 schema_example..."
# ./${BUILD_DIR}/examples/schema_example
