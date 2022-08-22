# CMakeList.txt : CMake project for foxy_cpp/src, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.23)
set(TARGET_NAME "foxy")

set(HEADER_FILES
    #../include/foxy/foxy.hpp
    ../include/foxy/core/app.hpp
    ../include/foxy/util/log.hpp
    ../include/foxy/util/util.hpp
    ../include/foxy/util/wait.hpp
    )

set(SOURCE_FILES
    core/app.cpp)

# Add source to this project's executable.
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
#target_compile_options(${TARGET_NAME} PUBLIC "/experimental:module" "/Zc:preprocessor")
target_compile_definitions(${TARGET_NAME}
    PRIVATE
    _CRT_SECURE_NO_WARNINGS=1
    WIN32_LEAN_AND_MEAN=1
    PUBLIC
    $<$<CONFIG:debug>:FLUGEL_ENABLE_ASSERTS=1>
    $<$<CONFIG:relwithdebinfo>:FLUGEL_ENABLE_ASSERTS=1>
)

target_include_directories(${TARGET_NAME} PUBLIC "../include")

target_precompile_headers(${TARGET_NAME} PRIVATE "../include/foxy/internal/fxpch.hpp")

# TODO: Add tests and install targets if needed.

# ===================================================
# DEPENDENCIES - VCPKG SUGGESTED
# ===================================================

# Boost
set(Boost_USE_STATIC_RUNTIME ON)
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
target_compile_definitions(${TARGET_NAME} PUBLIC FLUGEL_USE_VULKAN=1)

# ===================================================
# DEPENDENCIES - SUBMODULES
# ===================================================

# GLFW
target_sources(${TARGET_NAME} PRIVATE ${GLFW_FILES})
target_compile_definitions(${TARGET_NAME} PUBLIC FLUGEL_USE_GLFW=1 GLFW_INCLUDE_NONE=1)
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
    PUBLIC
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

# ImGUI
target_include_directories(${TARGET_NAME}
    PUBLIC
    "../extern/imgui-docking"
    )
target_sources(${TARGET_NAME}
    PRIVATE
    "../extern/imgui-docking/imgui_demo.cpp"
    "../extern/imgui-docking/imgui_draw.cpp"
    "../extern/imgui-docking/imgui_tables.cpp"
    "../extern/imgui-docking/imgui_widgets.cpp"
    "../extern/imgui-docking/imgui.cpp"
    )
# IMGUI RENDER
target_sources(${TARGET_NAME} PRIVATE "../extern/imgui-docking/backends/imgui_impl_vulkan.cpp")
# IMGUI WINDOW
target_sources(${TARGET_NAME} PRIVATE "../extern/imgui-docking/backends/imgui_impl_glfw.cpp")
