cmake_minimum_required(VERSION 3.24)
project(stb)
set(TARGET_NAME "stb")

# ===================================================
# LIBRARY
# ===================================================
set(SOURCE_FILES
    "stb_image.cpp"
    )
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
# target_compile_options(${TARGET_NAME} PRIVATE "/bigobj" "/std:c++latest" "/experimental:module")
# target_compile_definitions(${TARGET_NAME}
#                            PRIVATE 
#                            PUBLIC 
# )
# target_precompile_headers(${TARGET_NAME} PUBLIC "koyote/internal/pch.hpp")
target_include_directories(${TARGET_NAME} PUBLIC "include")

# ===================================================
# DEPENDENCIES
# ===================================================
