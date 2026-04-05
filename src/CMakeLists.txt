# src/CMakeLists.txt - 构建 ess_orm 库

file(GLOB_RECURSE ORM_SOURCES "ess/orm/*.cpp")

# 创建库目标
add_library(
  ess_orm 
  ${ORM_SOURCES}
)

target_include_directories(
  ess_orm
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../configs
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  ess_orm
  PRIVATE
  ${SQLITE3_TARGET}
)

target_compile_options(
  ess_orm
  PRIVATE
  -O3
  -g 
  -fno-omit-frame-pointer
  -fsanitize=address
  -fsanitize=undefined
)

target_compile_definitions(
  ess_orm
  PRIVATE
  $<$<BOOL:${BUILD_SHARED_LIBS}>:ESS_ORM_EXPORTS>
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:ESS_ORM_STATIC>
)

target_link_options(
  ess_orm
  PRIVATE
  -fsanitize=address
  -fsanitize=undefined
)

# 包含标准安装目录
include(GNUInstallDirs)

# 安装库目标
install(TARGETS ess_orm
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/ess/orm
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ess
  FILES_MATCHING PATTERN "*.hpp"
)

# 生成并安装简化的 CMake 包配置文件
include(CMakePackageConfigHelpers)

# 创建配置文件的安装目录
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ess_orm)

# 生成版本文件
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# 生成简化的配置文件
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfig.cmake "
# ess_orm CMake 配置文件
# 简化的配置文件，设置变量供用户使用

set(ESS_ORM_FOUND TRUE)
set(ESS_ORM_VERSION \"${PROJECT_VERSION}\")
set(ESS_ORM_INCLUDE_DIRS \"\${CMAKE_CURRENT_LIST_DIR}/../../include\")

# 尝试查找库文件
find_library(ESS_ORM_LIBRARY
  NAMES ess_orm
  PATHS \"\${CMAKE_CURRENT_LIST_DIR}/..\"
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)

if(NOT ESS_ORM_LIBRARY)
  # 如果找不到，设置库名让链接器查找
  set(ESS_ORM_LIBRARY ess_orm)
endif()

set(ESS_ORM_LIBRARIES \${ESS_ORM_LIBRARY})

# 向后兼容
set(ess_orm_FOUND \${ESS_ORM_FOUND})
set(ess_orm_VERSION \${ESS_ORM_VERSION})
set(ess_orm_INCLUDE_DIRS \${ESS_ORM_INCLUDE_DIRS})
set(ess_orm_LIBRARIES \${ESS_ORM_LIBRARIES})

# 提供使用宏
macro(ess_orm_use_target target_name)
  target_include_directories(\${target_name} PRIVATE \${ESS_ORM_INCLUDE_DIRS})
  target_link_libraries(\${target_name} PRIVATE \${ESS_ORM_LIBRARIES})
endmacro()

# 创建导入目标（可选）
if(NOT TARGET ess::orm AND ESS_ORM_LIBRARY AND EXISTS \"\${ESS_ORM_LIBRARY}\")
  add_library(ess::orm UNKNOWN IMPORTED)
  set_target_properties(ess::orm PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES \"\${ESS_ORM_INCLUDE_DIRS}\"
    IMPORTED_LOCATION \"\${ESS_ORM_LIBRARY}\"
  )
endif()

message(STATUS \"Found ess_orm \${ESS_ORM_VERSION}\")
")

# 安装配置文件
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ess_orm
)

# 生成并安装 pkg-config 文件
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ess_orm.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/ess_orm.pc
  @ONLY
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ess_orm.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
