add_executable(progress progress.cpp)
target_link_libraries(progress PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv> pthread dl)