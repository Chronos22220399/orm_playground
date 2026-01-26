#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SQLite::SQLite3" for configuration "Release"
set_property(TARGET SQLite::SQLite3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SQLite::SQLite3 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsqlite3.a"
  )

list(APPEND _cmake_import_check_targets SQLite::SQLite3 )
list(APPEND _cmake_import_check_files_for_SQLite::SQLite3 "${_IMPORT_PREFIX}/lib/libsqlite3.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
