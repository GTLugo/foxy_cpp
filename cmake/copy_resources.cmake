# ===================================================
# Copy resources folder into output dir - https://www.youtube.com/watch?v=sTuIRzrB2dA
# ===================================================
set(TMP_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tmp")
set(RES_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/${PROJECT_NAME}")
set(RES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/${PROJECT_NAME}")

add_custom_target("${PROJECT_NAME}_delete_tmp_out" ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${TMP_OUT_DIR}
    COMMENT "Cleaning output tmp folder: ${TMP_OUT_DIR}")
add_custom_target("${PROJECT_NAME}_delete_resources_out" ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${RES_OUT_DIR}
    COMMENT "Cleaning output resource folder: ${RES_OUT_DIR}")
add_custom_target("${PROJECT_NAME}_copy_resources_to_out" ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${RES_SOURCE_DIR}
    ${RES_OUT_DIR}
    COMMENT "Copying resource folder to output:\n${RES_SOURCE_DIR} -> ${RES_INTERM_DIR}")

# These will execute in reverse order due to dependency graph
add_dependencies("${PROJECT_NAME}" "${PROJECT_NAME}_copy_resources_to_out")
add_dependencies("${PROJECT_NAME}_copy_resources_to_out" "${PROJECT_NAME}_delete_resources_out")
add_dependencies("${PROJECT_NAME}_delete_resources_out" "${PROJECT_NAME}_delete_tmp_out")
