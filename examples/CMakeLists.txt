add_executable(format_test format_test.cc)
target_link_libraries(format_test PRIVATE fmt::fmt)

add_executable(ffmpeg_test ffmpeg_test.cc)
target_include_directories(ffmpeg_test PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(ffmpeg_test PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(ffmpeg_test PRIVATE ${FFMPEG_LIBRARIES})

add_executable(
    get_sei
    get_sei.cc
)
target_include_directories(get_sei PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(get_sei PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(get_sei PRIVATE ${FFMPEG_LIBRARIES})

add_executable(
    uv_timer_test
    uv_timer_test.cc
)
target_link_libraries(
        uv_timer_test
        PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
        pthread
        dl
)
