# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
# Based on structure of: https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/extended-project
cmake_minimum_required(VERSION 3.24)
#set(CMAKE_CXX_STANDARD 23)
project("foxy")
message(STATUS "Configuring: ${PROJECT_NAME}")

# ===================================================
# CONFIGURATION
# ===================================================
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(ignoreMe "${CMAKE_TOOLCHAIN_FILE}${Boost_NO_WARN_NEW_VERSIONS}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/bin/$<CONFIG>/${CMAKE_SYSTEM_NAME}")
  if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE $<$<OR:$<CONFIG:release>,$<CONFIG:minsizerel>>:TRUE>)
  endif()
endif()
set(FOXY_EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

# ===================================================
# DEPENDENCIES
# ===================================================
# GLFW
add_subdirectory(extern/glfw)
# GLM
add_subdirectory(extern/glm)
# SPDLOG
set(SPDLOG_ENABLE_PCH ON)
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_WCHAR_SUPPORT ON)
set(SPDLOG_WCHAR_TO_UTF8_SUPPORT ON)
add_subdirectory(extern/spdlog)
# ShaderC
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
add_subdirectory(extern/shaderc)

# ===================================================
# SUBDIRECTORIES
# ===================================================
# Compiled library code
add_subdirectory("src")
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # Compiled test app
  add_subdirectory("apps")
endif()

# ===================================================
# CMake Extensions
# ===================================================
include(cmake/copy_resources.cmake)
