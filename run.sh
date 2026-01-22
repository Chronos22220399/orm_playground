#!/bin/zsh

## ===== 自动初始化第三方库 =====
if [ ! -d "thirdparties/fmt" ]; then
    echo "📦 正在拉取 fmt 库..."
    mkdir -p thirdparties
    git clone --depth 1 https://github.com/fmtlib/fmt.git thirdparties/fmt
fi

# ===== 配置 =====
BUILD_DIR="build"
EXECUTABLE="orm"
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
        
        # 展开指定文件（默认 schema.cpp）
        FILE="${2:-src/schema.cpp}"
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
    build|b)
        cmake --build ${BUILD_DIR} -j 4
        exit $?
        ;;
    
    # 帮助
    help|h)
        echo "用法: $0 [命令]"
        echo "  (无参数)  完整构建并运行"
        echo "  debug|d   展开模板到 expanded.cpp"
        echo "  clean|c   清理构建目录"
        echo "  build|b   仅编译"
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

ctest --test-dir ${BUILD_DIR} --output-on-failure

# export ASAN_OPTIONS=print_stacktrace=1:debug=1:halt_on_error=1

./${BUILD_DIR}/${EXECUTABLE}
