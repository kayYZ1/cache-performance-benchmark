#ifndef CACHE_BANDWIDTH_TEST_H
#define CACHE_BANDWIDTH_TEST_H

#include <cstddef>
#include <cstdint>
#include <vector>

class CacheBandwidthTest {
private:
  std::vector<char *> allocated_memory;
  char *allocate_aligned(size_t size_bytes);

public:
  enum AccessPattern {
    SEQUENTIAL,
    STRIDE_64,
    STRIDE_128,
    STRIDE_256,
    STRIDE_512,
    STRIDE_1024,
    STRIDE_4096
  };

  struct BandwidthResult {
    size_t size_bytes;
    AccessPattern pattern;
    double bandwidth_gb_per_sec;
    double avg_cycles_per_access;
    uint64_t total_accesses;
    uint64_t total_cycles;
  };

  CacheBandwidthTest();
  ~CacheBandwidthTest();

  size_t get_stride(AccessPattern pattern);
  const char *get_pattern_name(AccessPattern pattern);
  BandwidthResult measure_read_bandwidth(size_t size_bytes,
                                         AccessPattern pattern);
  void run_comprehensive_test();
  BandwidthResult test_single(size_t size_bytes, AccessPattern pattern);
};

#endif
