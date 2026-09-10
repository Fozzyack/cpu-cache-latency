#include <emmintrin.h> // Provides x86 memory-fence and cache-control intrinsics.
#include <stdalign.h>  // Provides the alignas keyword.
#include <stdint.h>    
#include <stdio.h>     
#include <x86intrin.h> // Provides __rdtsc, __rdtscp, and other x86 intrinsics.

static inline uint64_t start_timer(void) {
    _mm_lfence();              // Finish earlier loads before starting the timer.
    uint64_t t = __rdtsc();    // Read the CPU's timestamp counter.
    _mm_lfence();              // Keep later loads from moving before this timestamp.
    return t;
}

static inline uint64_t stop_timer(void) {
    unsigned aux;              // Receives CPU identification information; unused here.
    uint64_t t = __rdtscp(&aux); // Read the counter after earlier instructions complete.
    _mm_lfence();               // Keep later instructions from moving before this timestamp.
    return t;
}

int main(void) {
    alignas(64) uint64_t value = 123; // Align value to a typical 64-byte cache-line boundary.
    volatile uint64_t result;         // Prevent the compiler from discarding the measured load.

    result = value; // Warm value's cache line so the next access should be cache-resident.

    // Measure an access to the warmed cache line, which will likely be served by L1.
    uint64_t start = start_timer();
    result = value;
    uint64_t l1_cycles = stop_timer() - start;

    _mm_clflush((void *)&value); // Evict value's cache line from the processor's caches.
    _mm_mfence();                // Wait until the cache-line flush has completed.

    // Measure the access after eviction; it must fetch the cache line again.
    start = start_timer();
    result = value;
    uint64_t ram_cycles = stop_timer() - start;

    printf("L1-ish access: %lu cycles\n", l1_cycles);
    printf("Flushed access: %lu cycles\n", ram_cycles);
    printf("%ld\n", result); // Use result so the compiler stays happy.

    return 0;
}
