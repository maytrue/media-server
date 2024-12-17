cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(uv)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/thirdparty/build/lib/cmake)
find_package(PkgConfig REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)

IF(LIBUV_FOUND)
    message("libuv found")
    SET(LIBUV_INCLUDE_DIRS "${LIBUV_INCLUDE_DIR}")
    SET(LIBUV_LIBRARIES "${LIBUV_LIBRARY}")
ENDIF()

# pkg_check_modules(UV REQUIRED libuv)

#include_directories(/usr/local/Cellar/libuv/1.18.0/include)
#link_directories(/usr/local/Cellar/libuv/1.18.0/lib)
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/thirdparty/build/include
    ${CMAKE_SOURCE_DIR}/thirdparty/IpQuery_QQWry
    ${CMAKE_SOURCE_DIR}/thirdparty/uWebSockets/src
    ${CMAKE_SOURCE_DIR}/thirdparty/uWebSockets/uSockets/src
    ${CMAKE_SOURCE_DIR}/thirdparty/json/include
    ${CMAKE_SOURCE_DIR}/thirdparty/spdlog/include
    # ${FFMPEG_INCLUDE_DIRS}
)

link_directories(
    ${CMAKE_SOURCE_DIR}/thirdparty/build/lib
    ${CMAKE_SOURCE_DIR}/thirdparty/uWebSockets/uSockets/
    # ${FFMPEG_LIBRARY_DIRS}
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(helloworld)
add_subdirectory(libuv)
add_subdirectory(inter-thread-communication)
add_subdirectory(examples/ip-test)
# add_subdirectory(examples/websocket)
add_subdirectory(examples/json)
add_subdirectory(rtp_rtcp)
add_subdirectory(examples)