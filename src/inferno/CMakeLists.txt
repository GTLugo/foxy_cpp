cmake_minimum_required(VERSION 3.24)
set(TARGET_NAME "inferno")
message(STATUS "Configuring ${PROJECT_NAME} sub-project: ${TARGET_NAME}")

# ===================================================
# LIBRARY
# ===================================================
set(SOURCE_FILES
    "inferno/glfw/glfw.cpp"
    "inferno/glfw/context.cpp"
    "inferno/window.cpp"
)
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PRIVATE "/bigobj" "/std:c++latest" "/experimental:module")
target_compile_definitions(${TARGET_NAME}
    PRIVATE _CRT_SECURE_NO_WARNINGS=1 
            WIN32_LEAN_AND_MEAN=1
    PUBLIC $<$<CONFIG:debug>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
           $<$<CONFIG:relwithdebinfo>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
)
target_precompile_headers(${TARGET_NAME} PUBLIC "inferno/internal/pch.hpp")
target_include_directories(${TARGET_NAME} PUBLIC ".")

# ===================================================
# DEPENDENCIES
# ===================================================
# Koyote
target_include_directories(${TARGET_NAME} PUBLIC "../koyote")
target_link_libraries(${TARGET_NAME} PUBLIC koyote)
# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${TARGET_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PRIVATE ${Vulkan_LIBRARIES})
# GLFW
target_include_directories(${TARGET_NAME} PRIVATE "${FOXY_EXTERN_DIR}/glfw/include")
target_link_libraries(${TARGET_NAME} PRIVATE glfw)