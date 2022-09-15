# CMakeList.txt : CMake project for foxy_cpp/apps, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "example_app")

set(SOURCE_FILES "example_app.cpp")

# Add source to this project's executable.
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PUBLIC "/bigobj" "/std:c++latest" "/experimental:module" )

target_include_directories(${TARGET_NAME} PRIVATE "../include")
target_link_libraries(${TARGET_NAME} PRIVATE foxy)

# TODO: Add tests and install targets if needed.
