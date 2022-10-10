cmake_minimum_required(VERSION 3.24)
project("foxy" VERSION ${FOXY_VERSION})
set(TARGET_NAME "foxy")
#set(CMAKE_CXX_STANDARD 23)
message(STATUS "Configuring ${PROJECT_NAME} internal project: ${TARGET_NAME}")

# ===================================================
# CONFIGURATION
# ===================================================
option(FOXY_PERF_TITLE "Performance stats displayed in title" OFF)
configure_file("foxy/foxy_config.hpp.in" "foxy/foxy_config.hpp")

# ===================================================
# LIBRARY
# ===================================================
set(SOURCE_FILES
    "${FOXY_EXTERN_DIR}/../cpp.hint"
    "foxy/icon.rc"
    "foxy/app.cpp"
)
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PRIVATE "/bigobj" "/std:c++latest" "/experimental:module")
target_compile_definitions(${TARGET_NAME}
    PRIVATE _CRT_SECURE_NO_WARNINGS=1
            WIN32_LEAN_AND_MEAN=1
    PUBLIC $<$<CONFIG:debug>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1>
           $<$<CONFIG:relwithdebinfo>:FOXY_ENABLE_ASSERTS=1;FOXY_DEBUG_MODE=1;FOXY_RELDEB_MODE=1>
)
if(FOXY_PERF_TITLE)
  target_compile_definitions(${TARGET_NAME} PRIVATE FOXY_PERF_TITLE=1)
endif()
target_precompile_headers(${TARGET_NAME} PUBLIC "foxy/internal/foxy_pch.hpp")
target_include_directories(${TARGET_NAME} PUBLIC "../../include" "." "${PROJECT_BINARY_DIR}") # added binary dir to allow searching for the config file)

# ===================================================
# DEPENDENCIES
# ===================================================
# Koyote
target_include_directories(${TARGET_NAME} PUBLIC "${FOXY_EXTERN_DIR}/koyote/include")
target_link_libraries(${TARGET_NAME} PUBLIC koyote)
# Ookami
target_include_directories(${TARGET_NAME} PUBLIC "../../ookami_render_engine")
target_link_libraries(${TARGET_NAME} PUBLIC ookami)
# Neko
target_include_directories(${TARGET_NAME} PUBLIC "../../neko_ecs")
target_link_libraries(${TARGET_NAME} PUBLIC neko)
# Inferno
target_include_directories(${TARGET_NAME} PUBLIC "../../inferno_window_library")
target_link_libraries(${TARGET_NAME} PUBLIC inferno)
