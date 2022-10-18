# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
# Based on structure of: https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/extended-project
cmake_minimum_required(VERSION 3.24)
set(FOXY_VERSION "0.1.1")
project("foxy" VERSION ${FOXY_VERSION})
message(STATUS "Configuring: ${PROJECT_NAME}")

# ===================================================
# CONFIGURATION
# ===================================================
set(FOXY_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FOXY_EXTERN_DIR ${FOXY_ROOT_DIR}/extern)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(FOXY_PERF_TITLE OFF)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/bin/$<CONFIG>/${CMAKE_SYSTEM_NAME}")
  if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE $<$<OR:$<CONFIG:release>,$<CONFIG:minsizerel>>:TRUE>)
  endif()
endif()

# ===================================================
# DEPENDENCIES
# ===================================================
add_subdirectory(extern)
# GLFW
add_subdirectory(extern/glfw)
# Koyote
add_subdirectory(extern/koyote_utils)
# Glslang
set(ENABLE_GLSLANG_INSTALL OFF)
set(ENABLE_GLSLANG_BINARIES OFF)
add_subdirectory(extern/glslang)
# STB
add_subdirectory(extern/stb)

# ===================================================
# SUBDIRECTORIES
# ===================================================
# Compiled library code
add_subdirectory("src")
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # Compiled test app
  add_subdirectory("example")
endif()

# ===================================================
# CMake Extensions
# ===================================================
include(cmake/copy_resources.cmake)
