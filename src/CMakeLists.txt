# CMakeList.txt : CMake project for foxy_cpp/src, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.23)
set(TARGET_NAME "foxy")

set(HEADER_FILES
  "../include/foxy/macros.hpp"
  "../include/foxy/log_macros.hpp"
)

set(SOURCE_FILES 
  "icon.rc"
  "core/app.ixx"
  "util/wait.ixx"
  "util/log.ixx")

# Add source to this project's executable.
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PUBLIC "/experimental:module")

target_include_directories(${TARGET_NAME} PUBLIC ../include)

# TODO: Add tests and install targets if needed.

# ===================================================
# DEPENDENCIES - VCPKG SUGGESTED
# ===================================================

# Boost
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED
    COMPONENTS
    thread
    random
    )
target_link_libraries(${TARGET_NAME}
    PUBLIC
    Boost::headers
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
message(STATUS "Using VULKAN!")

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
message(STATUS "Using GLFW!")

# GLM
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
    PUBLIC
    "../extern/easyloggingpp/src/easylogging++.cc"
    )

# ImGUI
target_include_directories(${TARGET_NAME}
    PUBLIC
    "../extern/imgui-docking"
    )
target_sources(${TARGET_NAME}
    PUBLIC
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
