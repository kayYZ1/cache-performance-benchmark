#ifndef CACHE_TLB_TEST_H
#define CACHE_TLB_TEST_H

#include <cstdint>
#include <vector>

class CacheTlbTest {
public:
  enum class AccessPattern { Normal, Stride };

  struct Result {
    double avg_cycles;
    double median_cycles;
    double avg_nanosecs;
    double median_nanosecs;
    std::uint64_t iterations;
  };

  Result measure_latency(AccessPattern ap);
  std::vector<Result> run_comprehensive_test();

private:
  std::vector<long long> tlb_miss_container;

  void populate_tlb_miss_container();
};

#endif
