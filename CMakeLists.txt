cmake_minimum_required(VERSION 2.6)

cmake_policy(SET CMP0048 NEW)

project("Bootable" LANGUAGES C VERSION 0.1.0)

if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wfatal-errors -std=gnu11")
endif (CMAKE_COMPILER_IS_GNUCC)

include_directories("include")

add_subdirectory("src/core")
add_subdirectory("src/lang")
add_subdirectory("src/util")

enable_testing()

set (CPACK_PACKAGE_NAME "Bootable")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "A utility for creating bootable disk images.")
set (CPACK_PACKAGE_VENDOR "Taylor Holberton")
set (CPACK_PACKAGE_VERSION_MAJOR ${Bootable_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${Bootable_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${Bootable_VERSION_PATCH})
set (CPACK_PACKAGE_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set (CPACK_PACKAGE_CONTACT "taylorcholberton@gmail.com")
include (CPack)
