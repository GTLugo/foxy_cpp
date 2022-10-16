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
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(FOXY_PERF_TITLE OFF)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/bin/$<CONFIG>/${CMAKE_SYSTEM_NAME}")
  if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE $<$<OR:$<CONFIG:release>,$<CONFIG:minsizerel>>:TRUE>)
  endif()
endif()
set(FOXY_EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

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
## DXC
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "")
#set(LLVM_APPEND_VC_REV ON CACHE BOOL "") 
#set(LLVM_DEFAULT_TARGET_TRIPLE "dxil-ms-dx" CACHE STRING "")
#set(LLVM_ENABLE_EH ON CACHE BOOL "") 
#set(LLVM_ENABLE_RTTI ON CACHE BOOL "") 
#set(LLVM_INCLUDE_DOCS OFF CACHE BOOL "") 
#set(LLVM_INCLUDE_EXAMPLES OFF CACHE BOOL "") 
#set(LLVM_INCLUDE_TESTS OFF CACHE BOOL "") 
#set(LLVM_OPTIMIZED_TABLEGEN OFF CACHE BOOL "") 
#set(LLVM_REQUIRES_EH ON CACHE BOOL "") 
#set(LLVM_REQUIRES_RTTI ON CACHE BOOL "") 
#set(LLVM_TARGETS_TO_BUILD "None" CACHE STRING "")
#set(LIBCLANG_BUILD_STATIC ON CACHE BOOL "") 
#set(CLANG_BUILD_EXAMPLES OFF CACHE BOOL "") 
#set(CLANG_CL OFF CACHE BOOL "") 
#set(CLANG_ENABLE_ARCMT OFF CACHE BOOL "") 
#set(CLANG_ENABLE_STATIC_ANALYZER OFF CACHE BOOL "") 
#set(CLANG_INCLUDE_TESTS OFF CACHE BOOL "") 
#set(HLSL_INCLUDE_TESTS OFF CACHE BOOL "") 
#set(ENABLE_SPIRV_CODEGEN ON CACHE BOOL "") 
#set(SPIRV_BUILD_TESTS OFF CACHE BOOL "")
#add_subdirectory(extern/dxc)

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
