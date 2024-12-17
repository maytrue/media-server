add_executable(uvstop uv_stop.cpp)
target_link_libraries(uvstop uv pthread dl)

add_executable(
    libuv_client
    libuv_client.cc
)

target_link_libraries(
    libuv_client
    PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
    pthread
    dl
)

add_executable(
    libuv_server
    libuv_server.cc
)

target_link_libraries(
    libuv_server
    PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
    pthread
    dl
)

