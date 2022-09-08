# CMakeList.txt : CMake project for foxy_cpp/src, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.23)
set(TARGET_NAME "foxy")

set(SOURCE_FILES
    "foxy/icon.rc"
    "foxy/util/util.cpp"
    "foxy/util/log.cpp"
    "foxy/core/app.cpp"
    "foxy/core/window.cpp"
    "foxy/core/event_system/event.cpp"
    "foxy/api/glfw/glfw.cpp"
    "foxy/api/glfw/context.cpp"
    "foxy/api/glfw/unique_window.cpp"
    "foxy/api/vulkan/context.cpp"
    "foxy/api/vulkan/swapchain.cpp"
    "foxy/ookami/renderer.cpp"
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

target_precompile_headers(${TARGET_NAME} PUBLIC "../include/foxy/internal/fxpch.hpp")

# TODO: Add tests and install targets if needed.

# ===================================================
# DEPENDENCIES - VCPKG SUGGESTED
# ===================================================

# Boost
# TODO: https://www.boost.org/doc/libs/1_80_0/tools/bcp/doc/html/index.html
#target_include_directories(${TARGET_NAME}
#    PUBLIC
#    "../extern/boost"
#    )
set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED  ON)
find_package(Boost REQUIRED
    COMPONENTS
    container
    coroutine
    system
    thread
    random
    )
target_link_libraries(${TARGET_NAME}
    PUBLIC
    Boost::headers
    Boost::container
    Boost::coroutine
    Boost::system
    Boost::thread
    Boost::random
    )
target_compile_definitions(${TARGET_NAME}
    PRIVATE
    BOOST_UUID_FORCE_AUTO_LINK=1
    )

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${TARGET_NAME}
    PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    )
target_link_libraries(${TARGET_NAME}
    PRIVATE
    ${Vulkan_LIBRARIES}
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

# VKFW
target_include_directories(${TARGET_NAME}
    PRIVATE
    "../extern/vkfw/include"
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

## ImGUI
#target_include_directories(${TARGET_NAME}
#    PRIVATE
#    "../extern/imgui"
#    )
#target_sources(${TARGET_NAME}
#    PRIVATE
#    "../extern/imgui/imgui_demo.cpp"
#    "../extern/imgui/imgui_draw.cpp"
#    "../extern/imgui/imgui_tables.cpp"
#    "../extern/imgui/imgui_widgets.cpp"
#    "../extern/imgui/imgui.cpp"
#    )
## IMGUI RENDER
#target_sources(${TARGET_NAME} PRIVATE "../extern/imgui/backends/imgui_impl_vulkan.cpp")
## IMGUI WINDOW
#target_sources(${TARGET_NAME} PRIVATE "../extern/imgui/backends/imgui_impl_glfw.cpp")
