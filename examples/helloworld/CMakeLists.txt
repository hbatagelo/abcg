cmake_minimum_required(VERSION 3.11)

string(TOLOWER ${GRAPHICS_API} SOURCE_SUBDIR)
add_subdirectory(${SOURCE_SUBDIR})
