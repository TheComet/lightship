set (TARGET_NAME lightship)
set (LIGHTSHIP_LIB_TYPE STATIC)
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
configure_file (include/Lightship/export.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/generated/Lightship/export.h)
configure_file (include/Lightship/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/generated/Lightship/config.h)

add_definitions (-DLIGHTSHIP_BUILDING)

define_source_files (
    GLOB_CPP_PATTERNS
        src/*.cpp
        src/Chat/*.cpp
        src/Network/*.cpp
    GLOB_H_PATTERNS
        include/Lightship/*.h
        include/Lightship/Chat/*.h
        include/Lightship/Network/*.h)
setup_library (${LIGHTSHIP_LIB_TYPE})

install (TARGETS lightship DESTINATION "lib")
