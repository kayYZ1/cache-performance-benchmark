#ifndef CACHE_LATENCY_TEST_H
#define CACHE_LATENCY_TEST_H

#include <cstddef>
#include <cstdint>
#include <vector>

class CacheLatencyTest {
private:
  struct Node {
    Node *next;
    char padding[56];
  };
  std::vector<Node *> allocated_memory;
  Node *create_pointer_chain(size_t size_bytes);

public:
  struct LatencyResult {
    size_t size_bytes;
    double avg_cycles;
    double avg_nanosecs;
    uint64_t iterations;
  };

  CacheLatencyTest();
  ~CacheLatencyTest();

  LatencyResult measure_latency(size_t size_bytes,
                                uint64_t chase_count = 10000000);
  std::vector<LatencyResult> run_comprehensive_test();
};

#endif
