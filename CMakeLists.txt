# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
# Based on structure of: https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/extended-project
cmake_minimum_required(VERSION 3.23)
project("foxy_cpp")
message(STATUS "Configuring: ${PROJECT_NAME}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/bin/$<CONFIG>/${CMAKE_SYSTEM_NAME}")
if(WIN32)
  set(CMAKE_WIN32_EXECUTABLE $<$<OR:$<CONFIG:release>,$<CONFIG:minsizerel>>:TRUE>)
endif()

# ===================================================
# Project tree
# ===================================================

# Compiled library code
add_subdirectory("src")

# Compiled test app
add_subdirectory("apps")

# ===================================================
# Copy resources folder into output dir - https://www.youtube.com/watch?v=sTuIRzrB2dA
# ===================================================
add_custom_target(flugel_delete_resources_out ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}"
    COMMENT "Cleaning output resource folder: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}")
add_custom_target(flugel_delete_resources_build ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    "${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}"
    COMMENT "Cleaning build resource folder: ${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}")
add_custom_target(flugel_copy_resources_to_build ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/res/${PROJECT_NAME}"
    "${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}"
    COMMENT "Copying resource folder to output:\n${CMAKE_CURRENT_SOURCE_DIR}/res/${PROJECT_NAME} -> ${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}")
add_custom_target(flugel_copy_resources_to_out ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/res/${PROJECT_NAME}"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}"
    COMMENT "Copying resource folder to output:\n${CMAKE_CURRENT_SOURCE_DIR}/res/${PROJECT_NAME} -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}")
add_custom_target(flugel_delete_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}/shaders"
    COMMENT "Deleting uncompiled shader folder from output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}/shaders")

add_custom_target(flugel_prepare_shader_folder ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}/shaders_out"
    COMMENT "Preparing shader output folder: ${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}/shaders_out")
add_custom_target(flugel_compile_shaders ALL
    COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/shader_compile.bat
    COMMENT "Compiling shaders")
add_custom_target(flugel_move_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}/shaders_out"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}/shaders"
    COMMENT "Copying compiled shader folder to output:\n${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}/shaders_out -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}/shaders")

# These will execute in reverse order due to dependency graph
add_dependencies("foxy" flugel_move_shaders)
add_dependencies(flugel_move_shaders flugel_compile_shaders)
add_dependencies(flugel_compile_shaders flugel_prepare_shader_folder)
add_dependencies(flugel_prepare_shader_folder flugel_delete_shaders)
add_dependencies(flugel_delete_shaders flugel_copy_resources_to_out)
add_dependencies(flugel_copy_resources_to_out flugel_copy_resources_to_build)
add_dependencies(flugel_copy_resources_to_build flugel_delete_resources_build)
add_dependencies(flugel_delete_resources_build flugel_delete_resources_out)

# ===================================================
# SUBMODULES
# ===================================================
# GLFW
add_subdirectory(extern/glfw)
# GLM
add_subdirectory(extern/glm)
# EasyLogging++
add_subdirectory(extern/easyloggingpp)
