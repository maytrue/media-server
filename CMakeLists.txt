cmake_minimum_required(VERSION 3.12)
project(uv)

find_package(PkgConfig REQUIRED)

pkg_check_modules(UV REQUIRED libuv)

#include_directories(/usr/local/Cellar/libuv/1.18.0/include)
#link_directories(/usr/local/Cellar/libuv/1.18.0/lib)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(helloworld)
add_subdirectory(uvstop)
add_subdirectory(inter-thread-communication)