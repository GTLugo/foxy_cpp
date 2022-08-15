# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
# Based on structure of: https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/extended-project
cmake_minimum_required(VERSION 3.23)

project("foxy_cpp")

# Compiled library code
add_subdirectory("src")

# Compiled test app
add_subdirectory("apps")
