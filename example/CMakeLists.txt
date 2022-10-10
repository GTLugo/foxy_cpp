cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "example_app")
#set(CMAKE_CXX_STANDARD 23)
message(STATUS "Configuring ${PROJECT_NAME} internal project: ${TARGET_NAME}")

# ===================================================
# EXECUTABLE
# ===================================================
set(SOURCE_FILES "example_app/example_app.cpp" )
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PRIVATE "/bigobj" "/std:c++latest" "/experimental:module")

# ===================================================
# DEPENDENCIES
# ===================================================
target_include_directories(${TARGET_NAME} PRIVATE "../include" ".")
target_link_libraries(${TARGET_NAME} PRIVATE foxy)
