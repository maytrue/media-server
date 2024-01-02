add_executable(uvstop uv_stop.cpp)
target_link_libraries(uvstop uv pthread dl)

add_executable(
    libuv_client
    libuv_client.cc
)

target_link_libraries(
    libuv_client
    uv
    pthread
    dl
)

add_executable(
    libuv_server
    libuv_server.cc
)

target_link_libraries(
    libuv_server
    uv
    pthread
    dl
)

