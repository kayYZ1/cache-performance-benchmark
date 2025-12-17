#include <iostream>

#include <getopt.h>

#include "core/bandwidth/CacheBandwidthTest.hpp"
#include "core/latency/CacheLatencyTest.hpp"
#include "core/tlb/CacheTLBTest.hpp"

namespace {
static constexpr struct option arg_options[] = {
    {"latency", no_argument, 0, 'l'},
    {"bandwidth", no_argument, 0, 'b'},
    {"tlb", no_argument, 0, 't'},
    {0, 0, 0, 0}};

// Validate null terminator at compile time
static_assert(arg_options[3].name == nullptr && arg_options[3].has_arg == 0 &&
                  arg_options[3].flag == nullptr && arg_options[3].val == 0,
              "arg_options must be null-terminated");

void runAllTests() {
  CacheLatencyTest cacheLatencyTest;
  cacheLatencyTest.run_comprehensive_test();

  CacheBandwidthTest cacheBandwidthTest;
  cacheBandwidthTest.run_comprehensive_test();

  CacheTlbTest cacheTlbTest;
  cacheTlbTest.run_comprehensive_test();
}
} // namespace

int main(int argc, char *argv[]) {
  std::cout << "Cache performance tests for AMD Ryzen 5 3500U" << std::endl;

  if (argc == 1) {
    runAllTests();
    return 0;
  }

  int opt;
  int opt_index = 0;

  while ((opt = getopt_long(argc, argv, "lbt", arg_options, &opt_index)) !=
         -1) {
    switch (opt) {
    case 'l': {
      CacheLatencyTest cache_latency_test;
      cache_latency_test.run_comprehensive_test();
      break;
    }
    case 'b': {
      CacheBandwidthTest cache_bandwidth_test;
      cache_bandwidth_test.run_comprehensive_test();
      break;
    }
    case 't': {
      CacheTlbTest cache_tlb_test;
      cache_tlb_test.run_comprehensive_test();
      break;
    }
    case '?':
      std::cerr << "Invalid option. Use: -l/--latency, -b/--bandwidth, "
                   "-t/--tlb, or no args for all tests"
                << std::endl;
      return 1;
    }
  }

  return 0;
}
