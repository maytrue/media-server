cmake_minimum_required(VERSION 3.12)
project(uv)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/thirdparty/build/lib/cmake)
find_package(PkgConfig REQUIRED)
find_package(libuv REQUIRED)

IF(LIBUV_FOUND)
    message("libuv found")
    SET(LIBUV_INCLUDE_DIRS "${LIBUV_INCLUDE_DIR}")
    SET(LIBUV_LIBRARIES "${LIBUV_LIBRARY}")
ENDIF()

# pkg_check_modules(UV REQUIRED libuv)

#include_directories(/usr/local/Cellar/libuv/1.18.0/include)
#link_directories(/usr/local/Cellar/libuv/1.18.0/lib)
include_directories(
    ${CMAKE_SOURCE_DIR}/thirdparty/build/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/thirdparty/build/lib
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(helloworld)
add_subdirectory(uvstop)
add_subdirectory(inter-thread-communication)