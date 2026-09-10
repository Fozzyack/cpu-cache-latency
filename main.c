#include <emmintrin.h> // Provides x86 memory-fence and cache-control intrinsics.
#include <inttypes.h>  // Provides portable printf macros for uint64_t.
#include <stdalign.h>  // Provides the alignas keyword.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>    // Provides qsort.
#include <x86intrin.h> // Provides __rdtsc, __rdtscp, and other x86 intrinsics.

#define SAMPLES 1000U

static inline uint64_t start_timer(void) {
  _mm_lfence();           // Finish earlier loads before starting the timer.
  uint64_t t = __rdtsc(); // Read the CPU's timestamp counter.
  _mm_lfence();           // Keep later loads from moving before this timestamp.
  return t;
}

static inline uint64_t stop_timer(void) {
  unsigned aux; // Receives CPU identification information; unused here.
  uint64_t t =
      __rdtscp(&aux); // Read the counter after earlier instructions complete.
  _mm_lfence();      // Keep later instructions from moving before this timestamp.
  return t;
}

static int compare_cycles(const void *left, const void *right) {
  const uint64_t lhs = *(const uint64_t *)left;
  const uint64_t rhs = *(const uint64_t *)right;

  return (lhs > rhs) - (lhs < rhs);
}

static uint64_t median(uint64_t *samples) {
  qsort(samples, SAMPLES, sizeof(*samples), compare_cycles);
  return samples[SAMPLES / 2U];
}

static uint64_t subtract_overhead(uint64_t cycles, uint64_t overhead) {
  return cycles > overhead ? cycles - overhead : 0U;
}

int main(void) {
  alignas(64) static volatile uint64_t value = 123;
  static volatile uint64_t result;
  uint64_t overhead_samples[SAMPLES];
  uint64_t cached_samples[SAMPLES];
  uint64_t flushed_samples[SAMPLES];

  for (unsigned sample = 0U; sample < SAMPLES; ++sample) {
    uint64_t start;

    start = start_timer();
    overhead_samples[sample] = stop_timer() - start;

    // Touch the value before timing so this access should be L1-resident.
    result = value;
    start = start_timer();
    result = value;
    cached_samples[sample] = stop_timer() - start;

    _mm_clflush((void *)&value);
    _mm_mfence();

    // This is a cold/flushed load, not a guaranteed DRAM-only measurement.
    start = start_timer();
    result = value;
    flushed_samples[sample] = stop_timer() - start;
  }

  const uint64_t overhead = median(overhead_samples);
  const uint64_t cached = median(cached_samples);
  const uint64_t flushed = median(flushed_samples);

  printf("Samples: %u\n", SAMPLES);
  printf("Timer overhead: min=%" PRIu64 ", median=%" PRIu64 " cycles\n",
         overhead_samples[0], overhead);
  printf("Cached access: min=%" PRIu64 ", median=%" PRIu64 " cycles\n",
         cached_samples[0], cached);
  printf("Flushed access: min=%" PRIu64 ", median=%" PRIu64 " cycles\n",
         flushed_samples[0], flushed);
  printf("Median minus overhead: cached=%" PRIu64 ", flushed=%" PRIu64
         " cycles\n",
         subtract_overhead(cached, overhead),
         subtract_overhead(flushed, overhead));
  printf("Result: %" PRIu64 "\n", result);

  return 0;
}
