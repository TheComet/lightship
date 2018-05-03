if (WIN32 OR CYGWIN)
    set (PLATFORM_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/platform/not-implemented")
elseif (UNIX)
    set (PLATFORM_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/platform/linux")
else ()
    set (PLATFORM_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/platform/not-implemented")
endif ()

set (TARGET_NAME lightship-server)
set (LIBS lightship)
set (INCLUDE_DIRS
    "include"
    "${CMAKE_SOURCE_DIR}/Lightship/include"
    "${CMAKE_BINARY_DIR}/Lightship/include/generated")
file (GLOB SOURCE_FILES
    "src/*.cpp"
    "${PLATFORM_SOURCE_DIR}/*.c")

setup_main_executable ()
set_output_directories (${CMAKE_BINARY_DIR}/bin LOCAL RUNTIME PDB)
