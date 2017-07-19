cmake_minimum_required (VERSION 2.8.8)
project("simplefs" C)

# In-source build protection
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# Compile in C99 with -Wall -Wextra plus -g for Debug or -O2 for Release
set_property(GLOBAL PROPERTY C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# Add libraries
add_library(utils STATIC "src/utils.c")
add_library(hash STATIC "src/hash.c")
add_library(fscore STATIC "src/filesystem_core.c")
add_library(fsapi STATIC "src/filesystem_api.c")
include_directories("src")

# Add source
add_executable(simplefs "src/main.c")

# Link
target_link_libraries(simplefs fsapi fscore hash utils)
