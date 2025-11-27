#ifndef CACHE_TLB_TEST_H
#define CACHE_TLB_TEST_H

#include <cstdint>
#include <vector>

/**
 * 4kb page size for this OS
 */

class CacheTlbTest {
private:
  std::vector<long long> tlb_miss_container;

  enum AccessPattern { NORMAL, STRIDE };

  void populate_tlb_miss_container();

public:
  struct Result {
    double avg_cycles;
    double avg_nanosecs;
    uint64_t iterations;
  };

  Result measure_latency(AccessPattern ap);

  std::vector<Result> run_comprehensive_test();
};

#endif
