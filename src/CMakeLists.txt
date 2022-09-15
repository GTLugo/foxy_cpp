# CMakeList.txt : CMake project for foxy_cpp/src, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "foxy")

set(SOURCE_FILES
    "foxy/icon.rc"
    "foxy/util/util.cpp"
    "foxy/util/log.cpp"
    "foxy/core/app.cpp"
    "foxy/core/window.cpp"
    "foxy/core/events/event.cpp"
    "foxy/core/threading/job_system.cpp"
    "foxy/core/window/glfw/glfw.cpp"
    "foxy/core/window/glfw/glfw_context.cpp"
    "foxy/ookami/renderer.cpp"
    "foxy/ookami/vulkan/vulkan_context.cpp"
    "foxy/ookami/vulkan/swapchain.cpp"
    "foxy/ookami/vulkan/pipeline.cpp"
    "foxy/ookami/vulkan/shader.cpp"
    #"foxy/koyote/ecs.cpp"
)

# Add source to this project's executable.
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PUBLIC "/bigobj" "/std:c++latest" "/experimental:module")
target_compile_definitions(${TARGET_NAME}
    PRIVATE
    _CRT_SECURE_NO_WARNINGS=1
    WIN32_LEAN_AND_MEAN=1
    PUBLIC
    $<$<CONFIG:debug>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
    $<$<CONFIG:relwithdebinfo>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
)

target_include_directories(${TARGET_NAME} PUBLIC "../include" ".")

target_precompile_headers(${TARGET_NAME} PUBLIC "../include/foxy/internal/foxy_pch.hpp")

# TODO: Add tests and install targets if needed.

# ===================================================
# DEPENDENCIES - VCPKG SUGGESTED
# ===================================================

# Vulkan
find_package(Vulkan REQUIRED
    COMPONENTS
    shaderc_combined
    )
target_include_directories(${TARGET_NAME}
    PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    )
target_link_libraries(${TARGET_NAME}
    PRIVATE
    ${Vulkan_LIBRARIES}
    Vulkan::shaderc_combined
    )
target_sources(${TARGET_NAME} PRIVATE ${VULKAN_FILES})
target_compile_definitions(${TARGET_NAME} PUBLIC FOXY_USE_VULKAN=1)


# ===================================================
# DEPENDENCIES - SUBMODULES
# ===================================================

# GLFW
target_sources(${TARGET_NAME} PRIVATE ${GLFW_FILES})
target_compile_definitions(${TARGET_NAME} PUBLIC FOXY_USE_GLFW=1)
target_include_directories(${TARGET_NAME}
    PRIVATE
    "../extern/glfw/include"
    )
target_link_libraries(${TARGET_NAME}
    PRIVATE
    glfw
    )

# GLM
target_compile_definitions(${TARGET_NAME} PUBLIC GLM_FORCE_RADIANS=1)
target_include_directories(${TARGET_NAME}
    PUBLIC
    "../extern/glm"
    )
target_link_libraries(${TARGET_NAME}
    PRIVATE
    glm::glm
    )

# EasyLogging++
target_include_directories(${TARGET_NAME}
    PUBLIC
    "../extern/easyloggingpp/src"
    )
target_sources(${TARGET_NAME}
    PRIVATE
    "../extern/easyloggingpp/src/easylogging++.cc"
    )

# BS Thread Pool
target_include_directories(${TARGET_NAME}
    PRIVATE
    "../extern/bs_thread_pool"
    )
