add_executable(
    echo_server_test
    echo_server_test.cc
)

target_link_libraries(
    echo_server_test
    -l:uSockets.a 
    -luv
    -lpthread
    -lz
    -ldl
)