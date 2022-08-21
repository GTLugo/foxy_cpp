function(compile_shader FILE_PATH FILE_NAME SHADER_TYPE)
  add_custom_target("${PROJECT_NAME}_comp_${FILE_NAME}_${SHADER_TYPE}" ALL
      COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/shader_compile.bat ${FILE_PATH} ${FILE_NAME} ${SHADER_TYPE}
      COMMENT "Compiling shader ${FILE_PATH}/${FILE_NAME}: ${SHADER_TYPE}")
  add_dependencies("${PROJECT_NAME}_end_shader_compilation" "${PROJECT_NAME}_comp_${FILE_NAME}_${SHADER_TYPE}")
  add_dependencies("${PROJECT_NAME}_comp_${FILE_NAME}_${SHADER_TYPE}" "${PROJECT_NAME}_start_shader_compilation")
endfunction()