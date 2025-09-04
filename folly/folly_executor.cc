#include <iostream>
#include <memory>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/executors/SerializedExecutor.h>
#include <folly/executors/StrandExecutor.h>
#include "util/spdlog_intializer.h"

using namespace rtcserver;

int main(int argc, char const *argv[])
{
    SpdlogInitializer::Init();
    LOG_INFO("Hello, World!");

    folly::ThreadedExecutor executor;
    executor.add([&] {
        LOG_INFO("Hello from thread pool executor!");
    });

    executor.add([&] {
        LOG_INFO("Hello from another thread pool executor!");
    });


    return 0;
}
