# CMakeList.txt : CMake project for foxy_cpp/src, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.23)
set(TARGET_NAME "foxy")

set(SOURCE_FILES "lib.ixx" )

# Add source to this project's executable.
add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_23)
target_compile_options(${TARGET_NAME} PUBLIC "/experimental:module")

target_include_directories(${TARGET_NAME} PUBLIC ../include)

set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED
    COMPONENTS
    container
    coroutine
    log
    system
    thread
    random
    )
target_link_libraries(${TARGET_NAME}
    PUBLIC
    Boost::headers
    Boost::container
    Boost::coroutine
    Boost::log
    Boost::system
    Boost::thread
    Boost::random
    )
target_compile_definitions(${TARGET_NAME}
    PRIVATE
    BOOST_UUID_FORCE_AUTO_LINK=1
    )


# TODO: Add tests and install targets if needed.
