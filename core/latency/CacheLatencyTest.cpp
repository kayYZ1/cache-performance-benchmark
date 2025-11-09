#include "CacheLatencyTest.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

#include "platform/cpu_utils.hpp"

#define CPU_GHZ 2.1;

CacheLatencyTest::CacheLatencyTest() {}

CacheLatencyTest::~CacheLatencyTest() {
  for (Node *ptr : allocated_memory) {
    delete[] ptr;
  }
}

CacheLatencyTest::Node *
CacheLatencyTest::create_pointer_chain(size_t size_bytes) {
  size_t num_nodes = size_bytes / sizeof(Node);
  if (num_nodes < 2)
    num_nodes = 2;

  Node *nodes = new Node[num_nodes];
  allocated_memory.push_back(nodes);

  std::vector<size_t> indices(num_nodes);
  for (size_t i = 0; i < num_nodes; ++i) {
    indices[i] = i;
  }

  // Fisher-Yates shuffle for randomization
  unsigned int seed = platform::rdtsc(); // Use TSC for seed
  for (size_t i = num_nodes - 1; i > 0; --i) {
    size_t j = rand_r(&seed) % (i + 1);
    std::swap(indices[i], indices[j]);
  }

  // Link nodes in shuffled order
  for (size_t i = 0; i < num_nodes - 1; ++i) {
    nodes[indices[i]].next = &nodes[indices[i + 1]];
  }

  // Complete the circular chain
  nodes[indices[num_nodes - 1]].next = &nodes[indices[0]];

  return &nodes[indices[0]];
}

CacheLatencyTest::LatencyResult
CacheLatencyTest::measure_latency(size_t size_bytes, uint64_t chase_count) {
  Node *head = create_pointer_chain(size_bytes);

  // Warm up the cache
  Node *p = head;
  for (uint64_t i = 0; i < 1000; ++i) {
    p = p->next;
  }

  uint64_t start = platform::rdtsc();
  for (uint64_t i = 0; i < chase_count; ++i) {
    p = p->next;
  }
  uint64_t end = platform::rdtsc();

  // Prevent the compiler for optimziing away the loop
  if (p == nullptr) {
    std::cout << "Impossible" << std::endl;
  }

  uint64_t total_cycles = end - start;
  double avg_cycles = static_cast<double>(total_cycles) / chase_count;

  // Estimate nanoseconds (assuming 2.1 GHz base clock for Ryzen 5 3500U)
  double avg_ns = avg_cycles / CPU_GHZ;

  LatencyResult result;
  result.size_bytes = size_bytes;
  result.avg_cycles = avg_cycles;
  result.avg_nanosecs = avg_ns;
  result.iterations = chase_count;

  return result;
}

std::vector<CacheLatencyTest::LatencyResult>
CacheLatencyTest::run_comprehensive_test() {
  std::vector<LatencyResult> results;

  std::cout << "Running cache latency tests..." << std::endl;

  // Test sizes covering all cache level boundaries up to 32MB
  // L1: ~32KB, L2: ~512KB, L3: ~4MB (Ryzen 5 3500U)
  std::vector<size_t> test_sizes = {
      // Below L1 cache
      1 * 1024,  // 1KB
      4 * 1024,  // 4KB
      8 * 1024,  // 8KB
      16 * 1024, // 16KB

      // L1 cache boundary (~32KB)
      24 * 1024, // 24KB
      32 * 1024, // 32KB (L1 data cache)
      40 * 1024, // 40KB

      // Between L1 and L2
      64 * 1024,  // 64KB
      128 * 1024, // 128KB
      256 * 1024, // 256KB

      // L2 cache boundary (~512KB)
      384 * 1024, // 384KB
      512 * 1024, // 512KB (L2 cache)
      640 * 1024, // 640KB

      // Between L2 and L3
      1 * 1024 * 1024, // 1MB
      2 * 1024 * 1024, // 2MB

      // L3 cache boundary (~4MB)
      3 * 1024 * 1024, // 3MB
      4 * 1024 * 1024, // 4MB (L3 cache)
      5 * 1024 * 1024, // 5MB

      // Above L3 cache
      8 * 1024 * 1024,  // 8MB
      16 * 1024 * 1024, // 16MB
      24 * 1024 * 1024, // 24MB
      32 * 1024 * 1024  // 32MB (RAM)
  };

  for (size_t size : test_sizes) {
    std::cout << "Testing " << (size / 1024) << " KB... " << std::flush;
    LatencyResult result = measure_latency(size);
    results.push_back(result);
    std::cout << std::fixed << std::setprecision(2) << result.avg_cycles
              << " cycles (" << result.avg_nanosecs << " ns)" << std::endl;
  }

  std::cout << "Cache latency tests completed!" << std::endl;
  return results;
}
