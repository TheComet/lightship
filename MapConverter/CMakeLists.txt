set (TARGET_NAME lightship-mapconverter)
set (LIBS lightship)
set (INCLUDE_DIRS
    "include"
    "${CMAKE_SOURCE_DIR}/Lightship/include"
    "${CMAKE_BINARY_DIR}/Lightship/include/generated")
define_source_files (RECURSE)
setup_main_executable ()
set_output_directories (${CMAKE_RUNTIME_OUTPUT_DIRECTORY} LOCAL RUNTIME PDB)
