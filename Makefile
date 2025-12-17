# Compiler to use
CC = clang++

# Compiler flags
# -std=c++11: Use C++11 standard
# -Wall -Wextra: Enable all warnings for robust code
# -O2: Optimize for performance
# -g: Include debugging symbols
# -march=native: Optimize for current CPU architecture
# -fno-omit-frame-pointer: Better profiling support
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g -march=native -fno-omit-frame-pointer -I.

# Target executable name
TARGET = cache_benchmark

# Source files
SOURCES = main.cpp core/bandwidth/CacheBandwidthTest.cpp core/latency/CacheLatencyTest.cpp core/tlb/CacheTLBTest.cpp

# Object files (derived from sources)
OBJECTS = $(SOURCES:.cpp=.o)

# CPU core to pin to (change if needed)
CPU_CORE = 0

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compile source files to object files
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

# Run the benchmark with optimal settings
# Usage: make run [ARGS="--latency"]
run: $(TARGET)
	@echo "Running cache benchmark on CPU core $(CPU_CORE)..."
	@taskset -c $(CPU_CORE) ./$(TARGET) $(ARGS)

# Run with elevated priority (requires sudo for real-time priority)
# Usage: make run-priority [ARGS="--bandwidth"]
run-priority: $(TARGET)
	@echo "Running with high priority on CPU core $(CPU_CORE)..."
	@sudo nice -n -20 taskset -c $(CPU_CORE) ./$(TARGET) $(ARGS)

# Check current CPU frequency governor
check-governor:
	@echo "Current CPU frequency governors:"
	@cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor 2>/dev/null || echo "Unable to read governors (may require root or not supported)"

# Display current CPU frequencies
check-freq:
	@echo "Current CPU frequencies:"
	@cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq 2>/dev/null | awk '{print $$1/1000 " MHz"}' || echo "Unable to read frequencies"

# Set CPU governor to performance (REQUIRES SUDO)
# This must be run separately: make set-performance
set-performance:
	@echo "Setting CPU governor to performance mode (requires sudo)..."
	@echo "You may need to run: sudo make set-performance"
	@sudo cpupower frequency-set -g performance 2>/dev/null || \
	(for cpu in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do \
		echo performance | sudo tee $$cpu > /dev/null; \
	done && echo "Performance mode set via sysfs") || \
	echo "Failed to set performance mode. Install cpupower or run with sudo."

# Restore CPU governor to default (usually powersave or schedutil)
restore-governor:
	@echo "Restoring CPU governor to powersave/schedutil..."
	@sudo cpupower frequency-set -g schedutil 2>/dev/null || \
	sudo cpupower frequency-set -g powersave 2>/dev/null || \
	(for cpu in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do \
		echo schedutil | sudo tee $$cpu > /dev/null 2>&1 || echo powersave | sudo tee $$cpu > /dev/null; \
	done && echo "Governor restored") || \
	echo "Failed to restore governor"

# Show system info relevant to cache testing
system-info:
	@echo "=== CPU Information ==="
	@lscpu | grep -E "Model name|CPU\(s\)|Thread|Core|Socket|L1|L2|L3" || echo "lscpu not available"
	@echo ""
	@echo "=== Cache Information ==="
	@getconf -a | grep CACHE || echo "getconf not available"
	@echo ""
	@echo "=== Current CPU Governor ==="
	@make check-governor
	@echo ""
	@echo "=== Current CPU Frequencies ==="
	@make check-freq

# Setup guide
setup-help:
	@echo "=== Cache Benchmark Setup Guide ==="
	@echo ""
	@echo "1. Build the benchmark:"
	@echo "   make"
	@echo ""
	@echo "2. Check current system state:"
	@echo "   make system-info"
	@echo ""
	@echo "3. Set performance mode (recommended for accurate results):"
	@echo "   sudo make set-performance"
	@echo ""
	@echo "4. Run the benchmark:"
	@echo "   make run                 # Normal run"
	@echo "   make run-priority        # High priority (requires sudo)"
	@echo ""
	@echo "5. After testing, restore governor:"
	@echo "   sudo make restore-governor"
	@echo ""
	@echo "=== Optional Checks ==="
	@echo "   make check-governor      # View current CPU governor"
	@echo "   make check-freq          # View current CPU frequencies"
	@echo ""
	@echo "=== Notes ==="
	@echo "- Performance mode prevents frequency scaling during tests"
	@echo "- taskset pins execution to CPU core $(CPU_CORE)"
	@echo "- Use 'make CPU_CORE=1 run' to pin to different core"

# Clean up generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean run run-priority check-governor check-freq set-performance restore-governor system-info setup-help
