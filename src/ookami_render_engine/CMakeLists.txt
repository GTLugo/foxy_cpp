cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "ookami")
#set(CMAKE_CXX_STANDARD 23)
message(STATUS "Configuring ${PROJECT_NAME} sub-project: ${TARGET_NAME}")

# ===================================================
# LIBRARY
# ===================================================
set(SOURCE_FILES
    "ookami/render_engine.cpp"
    "ookami/core/context.cpp"
    "ookami/core/swapchain.cpp"
    "ookami/core/pipeline.cpp"
    "ookami/core/shader.cpp"
    "ookami/core/low_level_renderer.cpp")

add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PRIVATE "/bigobj" "/std:c++latest" "/experimental:module")
target_compile_definitions(${TARGET_NAME}
    PRIVATE _CRT_SECURE_NO_WARNINGS=1
            WIN32_LEAN_AND_MEAN=1
    PUBLIC $<$<CONFIG:debug>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
           $<$<CONFIG:relwithdebinfo>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1;FOXY_RELEASE_MODE=1>
           $<$<CONFIG:release>:FOXY_ENABLE_ASSERTS=1;FOXY_RELEASE_MODE=1>
)
target_precompile_headers(${TARGET_NAME} PUBLIC "ookami/internal/pch.hpp")
target_include_directories(${TARGET_NAME} PUBLIC ".")

# ===================================================
# DEPENDENCIES
# ===================================================
# Koyote
target_include_directories(${TARGET_NAME} PUBLIC "${FOXY_EXTERN_DIR}/koyote/include")
target_link_libraries(${TARGET_NAME} PUBLIC koyote)
# Inferno
target_include_directories(${TARGET_NAME} PRIVATE "../inferno_window_library/include")
target_link_libraries(${TARGET_NAME} PRIVATE inferno)
# Vulkan
add_subdirectory(vulkan_static)
target_include_directories(${TARGET_NAME} PRIVATE "../vulkan_static/include")
target_link_libraries(${TARGET_NAME} PRIVATE vulkan_static)
## Glslang
#target_include_directories(${TARGET_NAME} PRIVATE "${FOXY_EXTERN_DIR}/glslang")
#target_link_libraries(${TARGET_NAME} PRIVATE glslang)
# GLFW
target_include_directories(${TARGET_NAME} PRIVATE "${FOXY_EXTERN_DIR}/glfw/include")
target_link_libraries(${TARGET_NAME} PRIVATE glfw)