#include <iostream>
#include <memory>
#include <folly/executors/ThreadedExecutor.h>

int main(int argc, char const *argv[])
{
    std::cout << "Hello, World!" << std::endl;
    folly::ThreadedExecutor executor;
    executor.add([&] {
        std::cout << "Hello from thread pool executor!" << std::endl;
    });

    executor.add([&] {
        std::cout << "Hello from another thread!" << std::endl;
    });

    return 0;
}
