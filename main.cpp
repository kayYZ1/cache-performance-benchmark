#include <iostream>

#include "core/latency/CacheLatencyTest.cpp"

int main() {
    std::cout << "Cache performance tests for AMD Ryzen 5 3500U" << std::endl;
    std::cout << "============================================" << std::endl;

    CacheLatencyTest tester;
    auto results = tester.run_comprehensive_test();

    std::cout << "\nSummary of results:" << std::endl;
    std::cout << "==================" << std::endl;
    for (const auto& result : results) {
        std::cout << std::fixed << std::setprecision(2)
                  << (result.size_bytes / 1024) << " KB: "
                  << result.avg_cycles << " cycles ("
                  << result.avg_nanosecs << " ns)" << std::endl;
    }

    return 0;
}
