# src/CMakeLists.txt - 构建 ess_orm 库
file(GLOB_RECURSE ORM_SOURCES "ess/orm/*.cpp")

add_library(
  ess_orm 
  ${ORM_SOURCES}
)

target_include_directories(
  ess_orm
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
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
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS ess_orm
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/ess/orm
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ess
  FILES_MATCHING PATTERN "*.hpp"
)

if(APPLE)
  set(ESS_ORM_LIB_SUFFIX ".dylib")
elseif(WIN32)
  set(ESS_ORM_LIB_SUFFIX ".dll")
else()
  set(ESS_ORM_LIB_SUFFIX ".so")
endif()

install(DIRECTORY DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ess_orm)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfig.cmake "
set(ESS_ORM_FOUND TRUE)
set(ESS_ORM_VERSION \"${PROJECT_VERSION}\")
get_filename_component(ESS_ORM_CONFIG_DIR \"\${CMAKE_CURRENT_LIST_DIR}\" ABSOLUTE)
set(ESS_ORM_INCLUDE_DIRS \"\${ESS_ORM_CONFIG_DIR}/../../../include\")
if(EXISTS \"\${ESS_ORM_CONFIG_DIR}/../../libess_orm${ESS_ORM_LIB_SUFFIX}\")
  set(ESS_ORM_LIBRARY \"\${ESS_ORM_CONFIG_DIR}/../../libess_orm${ESS_ORM_LIB_SUFFIX}\")
else()
  set(ESS_ORM_LIBRARY \"\${ESS_ORM_CONFIG_DIR}/../../libess_orm.a\")
endif()
add_library(ess::orm SHARED IMPORTED)
set_target_properties(ess::orm PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES \"\${ESS_ORM_INCLUDE_DIRS}\"
  IMPORTED_LOCATION \"\${ESS_ORM_LIBRARY}\"
)
set(ESS_ORM_LIBRARIES ess::orm)
set(ess_orm_FOUND \${ESS_ORM_FOUND})
set(ess_orm_VERSION \${ESS_ORM_VERSION})
set(ess_orm_INCLUDE_DIRS \${ESS_ORM_INCLUDE_DIRS})
set(ess_orm_LIBRARIES \${ESS_ORM_LIBRARIES})
message(STATUS \"Found ess_orm \${ESS_ORM_VERSION}\")
")

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/ess_ormConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ess_orm
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ess_orm.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/ess_orm.pc
  @ONLY
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/ess_orm.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
