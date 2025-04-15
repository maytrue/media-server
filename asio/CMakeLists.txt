
add_executable(udp_server_test udp_server_test.cc)
target_link_libraries(udp_server_test PRIVATE Boost::asio webrtc spdlog::spdlog)