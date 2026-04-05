
# ess_orm CMake 配置文件
# 简化的配置文件，设置变量供用户使用

set(ESS_ORM_FOUND TRUE)
set(ESS_ORM_VERSION "0.0.1")
set(ESS_ORM_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../include")

# 尝试查找库文件
find_library(ESS_ORM_LIBRARY
  NAMES ess_orm
  PATHS "${CMAKE_CURRENT_LIST_DIR}/.."
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)

if(NOT ESS_ORM_LIBRARY)
  # 如果找不到，设置库名让链接器查找
  set(ESS_ORM_LIBRARY ess_orm)
endif()

set(ESS_ORM_LIBRARIES ${ESS_ORM_LIBRARY})

# 向后兼容
set(ess_orm_FOUND ${ESS_ORM_FOUND})
set(ess_orm_VERSION ${ESS_ORM_VERSION})
set(ess_orm_INCLUDE_DIRS ${ESS_ORM_INCLUDE_DIRS})
set(ess_orm_LIBRARIES ${ESS_ORM_LIBRARIES})

# 提供使用宏
macro(ess_orm_use_target target_name)
  target_include_directories(${target_name} PRIVATE ${ESS_ORM_INCLUDE_DIRS})
  target_link_libraries(${target_name} PRIVATE ${ESS_ORM_LIBRARIES})
endmacro()

# 创建导入目标（可选）
if(NOT TARGET ess::orm AND ESS_ORM_LIBRARY AND EXISTS "${ESS_ORM_LIBRARY}")
  add_library(ess::orm UNKNOWN IMPORTED)
  set_target_properties(ess::orm PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ESS_ORM_INCLUDE_DIRS}"
    IMPORTED_LOCATION "${ESS_ORM_LIBRARY}"
  )
endif()

message(STATUS "Found ess_orm ${ESS_ORM_VERSION}")
