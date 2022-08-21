# ===================================================
# Copy resources folder into output dir - https://www.youtube.com/watch?v=sTuIRzrB2dA
# ===================================================
set(RES_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}")
set(RES_INTERM_DIR "${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}")
set(RES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/${PROJECT_NAME}")
set(RES_SHADER_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}/shaders")
set(RES_SHADER_INTERM_DIR "${CMAKE_CURRENT_BINARY_DIR}/res/${PROJECT_NAME}/shaders_out")

add_custom_target("${PROJECT_NAME}_delete_resources_out" ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${RES_OUT_DIR}
    COMMENT "Cleaning output resource folder: ${RES_OUT_DIR}")
add_custom_target("${PROJECT_NAME}_delete_resources_build" ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${RES_INTERM_DIR}
    COMMENT "Cleaning build resource folder: ${RES_INTERM_DIR}")
add_custom_target("${PROJECT_NAME}_copy_resources_to_build" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RES_SOURCE_DIR}
    ${RES_INTERM_DIR}
    COMMENT "Copying resource folder to intermediate output:\n${RES_SOURCE_DIR} -> ${RES_INTERM_DIR}")
add_custom_target("${PROJECT_NAME}_copy_resources_to_out" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RES_SOURCE_DIR}
    ${RES_OUT_DIR}
    COMMENT "Copying resource folder to output:\n${RES_SOURCE_DIR} -> ${RES_OUT_DIR}")
add_custom_target("${PROJECT_NAME}_delete_shaders" ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${RES_SHADER_OUT_DIR}
    COMMENT "Deleting uncompiled shader folder from output: ${RES_SHADER_OUT_DIR}")

add_custom_target("${PROJECT_NAME}_prepare_shader_folder" ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${RES_SHADER_INTERM_DIR}
    COMMENT "Preparing shader output folder: ${RES_SHADER_INTERM_DIR}")
add_custom_target("${PROJECT_NAME}_start_shader_compilation" ALL
    COMMENT "Starting shader compilation...")
add_custom_target("${PROJECT_NAME}_end_shader_compilation" ALL
    COMMENT "Finished shader compilation.")
add_custom_target("${PROJECT_NAME}_move_shaders" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RES_SHADER_INTERM_DIR}
    ${RES_SHADER_OUT_DIR}
    COMMENT "Copying compiled shader folder to output:\n${RES_SHADER_INTERM_DIR} -> ${RES_SHADER_OUT_DIR}")

include(cmake/compile_shaders.cmake)
# These will execute in reverse order due to dependency graph
add_dependencies("${PROJECT_NAME}" "${PROJECT_NAME}_move_shaders")
add_dependencies("${PROJECT_NAME}_move_shaders" "${PROJECT_NAME}_end_shader_compilation")

COMPILE_SHADER("res/${PROJECT_NAME}/shaders" "simple_shader" "vertex")
COMPILE_SHADER("res/${PROJECT_NAME}/shaders" "simple_shader" "fragment")

add_dependencies("${PROJECT_NAME}_start_shader_compilation" "${PROJECT_NAME}_prepare_shader_folder")
add_dependencies("${PROJECT_NAME}_prepare_shader_folder" "${PROJECT_NAME}_delete_shaders")
add_dependencies("${PROJECT_NAME}_delete_shaders" "${PROJECT_NAME}_copy_resources_to_out")
add_dependencies("${PROJECT_NAME}_copy_resources_to_out" "${PROJECT_NAME}_copy_resources_to_build")
add_dependencies("${PROJECT_NAME}_copy_resources_to_build" "${PROJECT_NAME}_delete_resources_build")
add_dependencies("${PROJECT_NAME}_delete_resources_build" "${PROJECT_NAME}_delete_resources_out")
