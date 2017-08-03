set (TARGET_NAME lightship-client)
set (LIBS lightship)
set (INCLUDE_DIRS
    "include"
    "${CMAKE_SOURCE_DIR}/lightship/include"
    "${CMAKE_BINARY_DIR}/lightship/include/generated")
define_source_files (RECURSE)
setup_main_executable ()
