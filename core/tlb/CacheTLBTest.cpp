#include "CacheTLBTest.hpp"
#include "platform/cpu_utils.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iomanip>

#define CPU_GHZ 2.1;

static int const NUM_OF_PAGE_ELEMENTS = 512;

void CacheTlbTest::populate_tlb_miss_container() {
  // Resize to a large array spanning multiple pages
  tlb_miss_container.resize(1000000);
}

CacheTlbTest::Result
CacheTlbTest::measure_latency(CacheTlbTest::AccessPattern ap) {
  populate_tlb_miss_container();

  uint64_t iterations = 10000000;
  uint64_t start = platform::rdtsc();

  if (ap == NORMAL) {
    for (uint64_t i = 0; i < iterations; ++i) {
      volatile long long x = tlb_miss_container[i % tlb_miss_container.size()];
    }
  } else { // STRIDE
    for (uint64_t i = 0; i < iterations; ++i) {
      size_t index = (i * NUM_OF_PAGE_ELEMENTS) % tlb_miss_container.size();
      volatile long long x = tlb_miss_container[index];
    }
  }

  uint64_t end = platform::rdtsc();

  uint64_t total_cycles = end - start;
  double avg_cycles = static_cast<double>(total_cycles) / iterations;
  double avg_ns = avg_cycles / CPU_GHZ;

  Result result;
  result.avg_cycles = avg_cycles;
  result.avg_nanosecs = avg_ns;
  result.iterations = iterations;

  return result;
}

std::vector<CacheTlbTest::Result>
CacheTlbTest::run_comprehensive_test() {
  std::vector<Result> results;

  std::cout << "Running TLB latency tests..." << std::endl;

  Result normal_result = measure_latency(NORMAL);
  Result stride_result = measure_latency(STRIDE);

  results.push_back(normal_result);
  results.push_back(stride_result);

  std::cout << "Normal Access: " << std::fixed << std::setprecision(2)
            << normal_result.avg_cycles << " cycles ("
            << normal_result.avg_nanosecs << " ns) | "
            << "Stride Access: " << stride_result.avg_cycles << " cycles ("
            << stride_result.avg_nanosecs << " ns)" << std::endl;

  std::cout << "TLB latency tests completed!" << std::endl;
  return results;
}
