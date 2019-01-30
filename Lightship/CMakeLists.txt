set (TARGET_NAME lightship)
set (LIGHTSHIP_LIB_TYPE SHARED)
set (INCLUDE_DIRS
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include/generated)

include (TestVisibilityMacros)
test_visibility_macros (LIGHTSHIP_HELPER_API_EXPORT LIGHTSHIP_HELPER_API_IMPORT LIGHTSHIP_HELPER_API_LOCAL)

# Define generated source files
configure_file (include/Lightship/config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/generated/Lightship/config.hpp)

define_source_files (
    GLOB_CPP_PATTERNS
        "src/*.cpp"
        "src/Chat/*.cpp"
        "src/Menu/*.cpp"
        "src/Network/*.cpp"
        "src/UserManager/*.cpp"
    GLOB_H_PATTERNS
        "include/Lightship/*.h"
        "include/Lightship/Chat/*.h"
        "include/Lightship/Menu/*.h"
        "include/Lightship/Network/*.h")
setup_library (${LIGHTSHIP_LIB_TYPE})

install (TARGETS lightship DESTINATION "lib")

target_compile_definitions(lightship
    PRIVATE
        LIGHTSHIP_BUILDING)
