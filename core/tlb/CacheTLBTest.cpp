#include "CacheTLBTest.hpp"
#include "platform/cpu_utils.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace {
constexpr double CPU_GHZ = 2.1;
constexpr std::size_t PAGE_SIZE = 4096;
constexpr std::size_t ELEMENTS_PER_PAGE = PAGE_SIZE / sizeof(long long);
constexpr std::size_t NUM_PAGES = 2048;
constexpr std::size_t NUM_ELEMENTS = NUM_PAGES * ELEMENTS_PER_PAGE;
constexpr std::uint64_t NUM_ITERATIONS = 1 << 23; // 8,388,608
constexpr std::size_t NUM_RUNS = 7;
} // namespace

void CacheTlbTest::populate_tlb_miss_container() {
  tlb_miss_container.resize(NUM_ELEMENTS);

  for (std::size_t i = 0; i < NUM_ELEMENTS; i += ELEMENTS_PER_PAGE) {
    tlb_miss_container[i] = 1;
  }
}

CacheTlbTest::Result CacheTlbTest::measure_latency(AccessPattern ap) {
  populate_tlb_miss_container();

  std::vector<double> cycles_per_run;
  cycles_per_run.reserve(NUM_RUNS);

  for (std::size_t run = 0; run < NUM_RUNS; ++run) {
    std::uint64_t iterations = NUM_ITERATIONS;
    std::uint64_t start = platform::rdtsc();

    if (ap == AccessPattern::Normal) {
      for (std::uint64_t i = 0; i < iterations; ++i) {
        std::size_t index = i & (NUM_ELEMENTS - 1);
        volatile long long x = tlb_miss_container[index];
        (void)x;
      }
    } else {
      for (std::uint64_t i = 0; i < iterations; ++i) {
        std::size_t index = (i * ELEMENTS_PER_PAGE) & (NUM_ELEMENTS - 1);
        volatile long long x = tlb_miss_container[index];
        (void)x;
      }
    }

    std::uint64_t end = platform::rdtsc();
    std::uint64_t total_cycles = end - start;
    double avg_cycles = static_cast<double>(total_cycles) / iterations;
    cycles_per_run.push_back(avg_cycles);
  }

  std::sort(cycles_per_run.begin(), cycles_per_run.end());

  double median_cycles = cycles_per_run[NUM_RUNS / 2];
  double avg_cycles =
      std::accumulate(cycles_per_run.begin(), cycles_per_run.end(), 0.0) /
      NUM_RUNS;

  Result result;
  result.avg_cycles = avg_cycles;
  result.median_cycles = median_cycles;
  result.avg_nanosecs = avg_cycles / CPU_GHZ;
  result.median_nanosecs = median_cycles / CPU_GHZ;
  result.iterations = NUM_ITERATIONS;

  return result;
}

std::vector<CacheTlbTest::Result> CacheTlbTest::run_comprehensive_test() {
  std::vector<Result> results;
  results.reserve(2);

  std::cout << "Running TLB latency tests (" << NUM_RUNS << " runs each)..."
            << std::endl;

  Result normal_result = measure_latency(AccessPattern::Normal);
  Result stride_result = measure_latency(AccessPattern::Stride);

  results.push_back(normal_result);
  results.push_back(stride_result);

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "Normal Access:  avg = " << normal_result.avg_cycles
            << " cycles, median = " << normal_result.median_cycles
            << " cycles (" << normal_result.median_nanosecs << " ns)"
            << std::endl;
  std::cout << "Stride Access:  avg = " << stride_result.avg_cycles
            << " cycles, median = " << stride_result.median_cycles
            << " cycles (" << stride_result.median_nanosecs << " ns)"
            << std::endl;

  std::cout << "TLB latency tests completed!" << std::endl;
  return results;
}
