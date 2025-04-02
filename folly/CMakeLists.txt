add_executable(
        folly_vector_test
        folly_vector_test.cc
)

target_link_libraries(
        folly_vector_test PRIVATE Folly::folly Folly::folly_deps Folly::follybenchmark Folly::folly_test_util)
