add_executable(uvstop uv_stop.cpp)
target_link_libraries(uvstop uv pthread dl)