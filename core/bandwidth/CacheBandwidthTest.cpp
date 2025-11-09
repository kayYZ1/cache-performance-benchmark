#include "CacheBandwidthTest.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>

#include "platform/cpu_utils.hpp"

CacheBandwidthTest::CacheBandwidthTest() {}

CacheBandwidthTest::~CacheBandwidthTest() {
  for (char *ptr : allocated_memory) {
    free(ptr);
  }
}

char *CacheBandwidthTest::allocate_aligned(size_t size_bytes) {
  char *ptr = nullptr;
  posix_memalign(reinterpret_cast<void **>(&ptr), 64, size_bytes);
  if (ptr) {
    allocated_memory.push_back(ptr);
    memset(ptr, 0, size_bytes);
  }
  return ptr;
}

size_t CacheBandwidthTest::get_stride(AccessPattern pattern) {
  switch (pattern) {
  case SEQUENTIAL:
    return 8; // 8 bytes (64-bit read)
  case STRIDE_64:
    return 64;
  case STRIDE_128:
    return 128;
  case STRIDE_256:
    return 256;
  case STRIDE_512:
    return 512;
  case STRIDE_1024:
    return 1024;
  case STRIDE_4096:
    return 4096;
  default:
    return 8;
  }
}

const char *CacheBandwidthTest::get_pattern_name(AccessPattern pattern) {
  switch (pattern) {
  case SEQUENTIAL:
    return "Sequential";
  case STRIDE_64:
    return "Stride 64B";
  case STRIDE_128:
    return "Stride 128B";
  case STRIDE_256:
    return "Stride 256B";
  case STRIDE_512:
    return "Stride 512B";
  case STRIDE_1024:
    return "Stride 1KB";
  case STRIDE_4096:
    return "Stride 4KB";
  default:
    return "Unknown";
  }
}

CacheBandwidthTest::BandwidthResult
CacheBandwidthTest::measure_read_bandwidth(size_t size_bytes,
                                           AccessPattern pattern) {
  char *buffer = allocate_aligned(size_bytes);
  if (!buffer) {
    std::cerr << "Failed to allocate memory for the buffer" << std::endl;
    return BandwidthResult{0, pattern, 0.0, 0.0, 0, 0};
  }

  size_t stride = get_stride(pattern);
  size_t num_accesses = size_bytes / stride;
  if (num_accesses < 1) {
    num_accesses = 1;
  }

  uint64_t iterations;
  if (size_bytes <= 64 * 1024) {
    iterations = 100000; // L1, L2
  } else if (size_bytes <= 4 * 1024 * 1024) {
    iterations = 10000; // L3
  } else {
    iterations = 1000; // DRAM
  }

  // Warm up
  volatile uint64_t dummy = 0;
  for (size_t i = 0; i < num_accesses; i += stride) {
    dummy += *reinterpret_cast<uint64_t *>(buffer + (i % size_bytes));
  }

  uint64_t start = platform::rdtsc();
  for (uint64_t iter = 0; iter < iterations; ++iter) {
    for (size_t offset = 0; offset < size_bytes; offset += stride) {
      dummy += *reinterpret_cast<volatile uint64_t *>(buffer + offset);
    }
  }
  uint64_t end = platform::rdtsc();

  // Prevent from optimizing
  if (dummy == 0xFFFFFFFFFFFFFFFF) {
    std::cout << "impossible" << std::endl;
  }

  uint64_t total_cycles = end - start;
  uint64_t total_accesses = iterations * num_accesses;
  double avg_cycles_per_access =
      static_cast<double>(total_cycles) / total_accesses;

  // Calculate bandwidth
  // Total bytes read = total_accesses * 8 bytes per access
  uint64_t total_bytes = total_accesses * 8;
  double cpu_ghz = 2.1; // Ryzen 5 3500U base clock
  double total_seconds = total_cycles / (cpu_ghz * 1e9);
  double bandwidth_gb_per_sec = (total_bytes / total_seconds) / 1e9;

  BandwidthResult result;
  result.size_bytes = size_bytes;
  result.pattern = pattern;
  result.bandwidth_gb_per_sec = bandwidth_gb_per_sec;
  result.avg_cycles_per_access = avg_cycles_per_access;
  result.total_accesses = total_accesses;
  result.total_cycles = total_cycles;

  return result;
}

void CacheBandwidthTest::run_comprehensive_test() {
  std::cout << "Running Cache Bandwidth Tests...\n";
  std::cout << "=================================\n\n";

  std::vector<size_t> test_sizes = {
      16 * 1024,        // 16 KB - L1
      32 * 1024,        // 32 KB - L1 boundary
      128 * 1024,       // 128 KB - L2
      512 * 1024,       // 512 KB - L2 boundary
      2 * 1024 * 1024,  // 2 MB - L3
      4 * 1024 * 1024,  // 4 MB - L3 boundary
      16 * 1024 * 1024, // 16 MB - RAM
      64 * 1024 * 1024  // 64 MB - RAM
  };

  std::vector<AccessPattern> patterns = {SEQUENTIAL, STRIDE_64, STRIDE_256,
                                         STRIDE_1024, STRIDE_4096};

  for (size_t size : test_sizes) {
    std::string size_str;
    if (size < 1024 * 1024) {
      size_str = std::to_string(size / 1024) + " KB";
    } else {
      size_str = std::to_string(size / (1024 * 1024)) + " MB";
    }

    std::cout << "\n--- Testing with " << size_str << " buffer ---\n";
    std::cout << std::setw(15) << "Pattern" << std::setw(18)
              << "Bandwidth (GB/s)" << std::setw(18) << "Cycles/Access"
              << std::endl;
    std::cout << std::string(51, '-') << std::endl;

    for (AccessPattern pattern : patterns) {
      BandwidthResult result = measure_read_bandwidth(size, pattern);

      std::cout << std::setw(15) << get_pattern_name(pattern) << std::setw(18)
                << std::fixed << std::setprecision(2)
                << result.bandwidth_gb_per_sec << std::setw(18) << std::fixed
                << std::setprecision(2) << result.avg_cycles_per_access
                << std::endl;
    }
  }

  std::cout << "\n\nAnalysis Guide:\n";
  std::cout << "===============\n";
  std::cout << "Sequential Access: Maximum theoretical bandwidth\n";
  std::cout << "  - L1: ~50-100 GB/s (per core)\n";
  std::cout << "  - L2: ~20-40 GB/s\n";
  std::cout << "  - L3: ~10-20 GB/s\n";
  std::cout << "  - RAM: ~15-25 GB/s (DDR4-2400 dual channel)\n\n";
  std::cout << "Strided Access: Shows cache line efficiency\n";
  std::cout
      << "  - Larger strides = lower bandwidth (fewer cache lines used)\n";
  std::cout << "  - 4KB stride = page-level access pattern\n";
}

CacheBandwidthTest::BandwidthResult
CacheBandwidthTest::test_single(size_t size_bytes, AccessPattern pattern) {
  return measure_read_bandwidth(size_bytes, pattern);
}
