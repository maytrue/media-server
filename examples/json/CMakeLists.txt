add_executable(
    json_test
    json_test.cc
)

target_link_libraries(json_test PRIVATE nlohmann_json::nlohmann_json)