add_executable(
    rtp_test
    rtp_test.cc
    rtp.cc
)

target_link_libraries(rtp_test PRIVATE spdlog::spdlog)