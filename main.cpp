#include <iostream>

#include "core/bandwidth/CacheBandwidthTest.hpp"
#include "core/latency/CacheLatencyTest.hpp"

int main() {
  std::cout << "Cache performance tests for AMD Ryzen 5 3500U" << std::endl;
  std::cout << "============================================" << std::endl;

  CacheLatencyTest cacheLatencyTest;
  auto results = cacheLatencyTest.run_comprehensive_test();

  std::cout << std::endl;

  CacheBandwidthTest cacheBandwidthTest;
  cacheBandwidthTest.run_comprehensive_test();

  return 0;
}
