cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "vulkan_static")

# ===================================================
# LIBRARY
# ===================================================
set(SOURCES 
  "include/vulkan/internal/static.cpp"
  #"include/vulkan/shader.cpp"
)
add_library(${TARGET_NAME} STATIC ${SOURCES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PRIVATE "/bigobj" "/std:c++latest" "/experimental:module")
target_compile_definitions(${TARGET_NAME}
  PRIVATE _CRT_SECURE_NO_WARNINGS=1
          WIN32_LEAN_AND_MEAN=1
  PRIVATE NOMINMAX=1
          $<$<CONFIG:debug>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
          $<$<CONFIG:relwithdebinfo>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1;FOXY_RELDEB_MODE=1>
          $<$<CONFIG:release>:FOXY_ENABLE_ASSERTS=1;FOXY_RELEASE_MODE=1>
)
target_precompile_headers(${TARGET_NAME} PUBLIC "include/vulkan/internal/pch.hpp")
target_include_directories(${TARGET_NAME} PUBLIC "include")
set_target_properties(${TARGET_NAME} PROPERTIES STATIC_LIBRARY_FLAGS "/ignore:4099")

# ===================================================
# DEPENDENCIES
# ===================================================
# Koyote
target_include_directories(${TARGET_NAME} PUBLIC "${FOXY_EXTERN_DIR}/koyote/include")
target_link_libraries(${TARGET_NAME} PUBLIC koyote)
# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${TARGET_NAME} SYSTEM PUBLIC "${Vulkan_INCLUDE_DIR}")
target_link_libraries(${TARGET_NAME} PUBLIC Vulkan::Vulkan)
# Glslang
target_include_directories(${TARGET_NAME} SYSTEM PUBLIC "${FOXY_EXTERN_DIR}/glslang")
target_link_libraries(${TARGET_NAME} PUBLIC glslang HLSL OSDependent OGLCompiler SPIRV)
## DXC
#target_include_directories(${TARGET_NAME} SYSTEM PUBLIC "${FOXY_EXTERN_DIR}/dxc/include")
#target_link_libraries(${TARGET_NAME} PUBLIC dxcompiler)