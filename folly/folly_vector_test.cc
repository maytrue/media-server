//
// Created by zouguowei on 2025/4/1.
//

#include <folly/Benchmark.h>
#include <folly/small_vector.h>
#include <vector>

using namespace std;
using namespace folly;

BENCHMARK(insertFrontVector) {
  // Let's insert 100 elements at the front of a vector
  vector<int> v;
  for (unsigned int i = 0; i < 100; ++i) {
    v.insert(v.begin(), i);
  }
}
BENCHMARK(insertBackVector) {
  // Let's insert 100 elements at the back of a vector
  vector<int> v;
  for (unsigned int i = 0; i < 100; ++i) {
    v.insert(v.end(), i);
  }
}

BENCHMARK(insertSmallVector) {
  small_vector<int> v;
  for (unsigned int i = 0; i < 1000; ++i) {
    // v.insert(v.end(), i);
    v.push_back(i);
  }
}

int main(int argc, char *argv[]) {
  runBenchmarks();
  return 0;
}
