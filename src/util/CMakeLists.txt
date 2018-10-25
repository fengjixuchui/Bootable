cmake_minimum_required(VERSION 2.6)

add_executable("bootable"
	"fstream.c"
	"memory.c"
	"pure64.c"
	"util.c")

target_link_libraries("bootable" "bootable-core" "bootable-lang")

set_target_properties("bootable" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
