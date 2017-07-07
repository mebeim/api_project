cmake_minimum_required (VERSION 2.8.8)
project("api_project" C)

# In-source build protection
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# Compile in C99 with all warnings
set_property(GLOBAL PROPERTY C_STANDARD 99)
add_definitions(-Wall)

# Add libraries
add_library(utils STATIC "src/utils.c")
add_library(fs_core STATIC "src/filesystem_core.c")
add_library(fs_api STATIC "src/filesystem_api.c")
target_include_directories(utils PUBLIC "src/")
target_include_directories(fs_core PUBLIC "src/")
target_include_directories(fs_api PUBLIC "src/")

# Add source
add_executable(api_project "src/main.c")

# Link
target_link_libraries(api_project utils fs_core fs_api)
