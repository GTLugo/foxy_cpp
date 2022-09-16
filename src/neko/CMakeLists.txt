cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "neko")
message(STATUS "Configuring ${PROJECT_NAME} sub-project: ${TARGET_NAME}")

# ===================================================
# LIBRARY
# ===================================================
set(SOURCE_FILES
    "inu/job_system.cpp"
    "neko/ecs.cpp"
)
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PRIVATE "/bigobj" "/std:c++latest" "/experimental:module")
target_compile_definitions(${TARGET_NAME}
    PRIVATE _CRT_SECURE_NO_WARNINGS=1
            WIN32_LEAN_AND_MEAN=1
    PUBLIC $<$<CONFIG:debug>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
           $<$<CONFIG:relwithdebinfo>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
)
target_precompile_headers(${TARGET_NAME} PUBLIC "neko/internal/pch.hpp")
target_include_directories(${TARGET_NAME} PUBLIC ".")

# ===================================================
# DEPENDENCIES
# ===================================================
# Koyote
target_include_directories(${TARGET_NAME} PUBLIC "../koyote")
target_link_libraries(${TARGET_NAME} PRIVATE koyote)
# BS Thread Pool
target_include_directories(${TARGET_NAME} PUBLIC "${FOXY_EXTERN_DIR}/bs_thread_pool")
