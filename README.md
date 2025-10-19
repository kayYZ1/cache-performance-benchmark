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
make setup-help

# Clean build artifacts
make clean
```

## Usage

### Running the Tests
Simply execute the compiled binary:
```bash
./main
```

The program will automatically run all configured performance tests and display results in real-time.

## Tests Performed

### 1. Cache Latency Test ✅
Measures the average time (in CPU cycles and nanoseconds) required to access memory at different sizes, revealing cache hierarchy boundaries.

**Test Sizes**: 1KB to 32MB covering all cache levels
- L1 Cache (~32KB boundary)
- L2 Cache (~512KB boundary)
- L3 Cache (~4MB boundary)
- Main Memory (above 4MB)

**Output**: Shows latency in CPU cycles and estimated nanoseconds for each test size.

### 2. ???

```
Cache performance tests for AMD Ryzen 5 3500U
============================================
Running cache latency tests...
Testing 1 KB... 4.71 cycles (2.24 ns)
Testing 4 KB... 3.69 cycles (1.76 ns)
...
Testing 4096 KB... 390.51 cycles (185.95 ns)
Cache latency tests completed!

Summary of results:
==================
1 KB: 4.71 cycles (2.24 ns)
4 KB: 3.69 cycles (1.76 ns)
...
```

## Understanding Results

### Latency Test Interpretation
- **Low latencies (3-10 cycles)**: L1 cache hits
- **Medium latencies (15-30 cycles)**: L2 cache hits
- **Higher latencies (50-150 cycles)**: L3 cache hits
- **High latencies (300+ cycles)**: Main memory access

Clear "steps" in the latency curve indicate cache level boundaries.

## Technical Details

- **Timing**: Uses CPU Time Stamp Counter (TSC) for high-precision measurements
- **Randomization**: Employs Fisher-Yates shuffle for cache conflict-free testing
- **Memory Management**: Automatic cleanup prevents memory leaks
- **Thread Safety**: Uses thread-safe random number generation

## Hardware Requirements

- AMD Ryzen processor (optimized for Ryzen 5 3500U)
- Linux operating system
- Sufficient RAM for large test sizes (32MB+ recommended)
