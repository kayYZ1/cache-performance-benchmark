#include <cstdint>

#ifndef PLATFORM_CPU_UTILS_H
#define PLATFORM_CPU_UTILS_H

namespace platform {
    /**
     * Reads the Time Stamp Counter (TSC) register using inline assembly.
     *
     * @return uint64_t The current value of the Time Stamp Counter
     * @note This function uses inline assembly and is x86-specific
     * @warning TSC may not be synchronized across CPU cores on some systems
     */
    inline uint64_t rdtsc() {
        unsigned int low, high;
        __asm__ __volatile__ ("rdtsc" : "=a" (low), "=d" (high));
        return ((uint64_t) high << 32) | low;
    }
}

#endif
