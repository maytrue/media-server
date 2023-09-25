add_executable(
    echo_server_test
    echo_server_test.cc
)

target_link_libraries(
    echo_server_test
    -luSockets
    -luv
    -lpthread
    -lz
    -ldl
)