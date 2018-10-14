add_executable(progress progress.cpp)
target_link_libraries(progress uv pthread dl)