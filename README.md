# Cache Performance Testing Suite

A comprehensive benchmarking tool for measuring CPU cache performance characteristics on AMD Ryzen processors.

## Overview

This tool measures various aspects of CPU cache performance including latency, bandwidth, and other cache-related metrics. It uses high-precision timing techniques to provide accurate measurements of memory access patterns across different cache levels.

## Building

### Prerequisites

- C++11 compatible compiler (tested with clang++)
- GNU Make
- Linux environment

### Build Commands

```bash
# Build the project
make

# Clean build artifacts
make clean
```

## Usage

### Running the Tests

Simply execute the compiled binary:

```bash
./cache_benchmark
```

The program will automatically run all configured performance tests and display results in real-time.

## Tests Performed

### 1. Cache Latency Test

Measures the average time (in CPU cycles and nanoseconds) required to access memory at different sizes, revealing cache hierarchy boundaries.

**Test Sizes**: 1KB to 32MB covering all cache levels

- L1 Cache (~32KB boundary)
- L2 Cache (~512KB boundary)
- L3 Cache (~4MB boundary)
- Main Memory (above 4MB)

**Output**: Shows latency in CPU cycles and estimated nanoseconds for each test size.

### 2. Cache Bandwidth Test

Measures the average bandwidth (in GB/s) for loading cache lines of different sizes, including sequential and strided access patterns.

**Test Sizes**: 16KB to 64MB covering both core (L1) and shared caches (L2, L3) as well as DRAM (32MB, 64MB)

- L1 Cache
- L2 Cache
- L3 Cache
- Main memory (4MB+)

**Output**: Shows average bandwidth for each test size in both sequential and strided access cases

## Understanding Results

### Latency Test Interpretation

- **Low latencies (3-10 cycles)**: L1 cache hits
- **Medium latencies (15-30 cycles)**: L2 cache hits
- **Higher latencies (50-150 cycles)**: L3 cache hits
- **High latencies (300+ cycles)**: Main memory access

Clear "steps" in the latency curve indicate cache level boundaries.

### Bandwidth Test Interpretation

- **High bandwidth (100+ GB/s)**: L1 cache access
- **Medium bandwidth (50-100 GB/s)**: L2 cache access
- **Lower bandwidth (10-50 GB/s)**: L3 cache access
- **Low bandwidth (<10 GB/s)**: Main memory access

Sequential access generally provides higher bandwidth than strided access due to improved cache locality.

## Technical Details

- **Timing**: Uses CPU Time Stamp Counter (TSC) for high-precision measurements
- **Randomization**: Employs Fisher-Yates shuffle for cache conflict-free testing
- **Memory Management**: Automatic cleanup prevents memory leaks
- **Thread Safety**: Uses thread-safe random number generation

## Hardware Requirements

- AMD Ryzen processor (optimized for Ryzen 5 3500U)
- Linux operating system
- Sufficient RAM for large test sizes (32MB+ recommended)
