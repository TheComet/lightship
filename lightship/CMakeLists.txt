set (TARGET_NAME lightship)
set (LIGHTSHIP_LIB_TYPE SHARED)
set (INCLUDE_DIRS
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include/generated)

# Need to set LIGHTSHIP_PLATFORM for dllimport/dllexport
if (WIN32)
    set (LIGHTSHIP_PLATFORM "WINDOWS")
elseif (APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (LIGHTSHIP_PLATFORM "OSX")
elseif (IOS)
    set (LIGHTSHIP_PLATFORM "IOS")
elseif (UNIX)
    set (LIGHTSHIP_PLATFORM "LINUX")
else ()
    set (LIGHTSHIP_PLATFORM "ANDROID")
endif ()

# Define generated source files
configure_file (include/lightship/export.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/generated/lightship/export.h)
configure_file (include/lightship/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/generated/lightship/config.h)

add_definitions (-DLIGHTSHIP_BUILDING)

define_source_files (
    GLOB_CPP_PATTERNS src/*.cpp src/Network/*.cpp
    GLOB_H_PATTERNS include/lightship/*.h)
setup_library (SHARED)

install (TARGETS lightship DESTINATION "lib")
