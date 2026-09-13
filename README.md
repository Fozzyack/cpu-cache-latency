# cpu-cache-latency

A small benchmark to compare a cache-resident load with a flushed memory load.

```bash
make # builds the benchmark
make run # builds and runs the benchmark
```

The benchmark takes 1,000 samples and reports the minimum and median. The
timer-adjusted value is only a rough comparison because the timer and fences
can cost more than an L1 hit, especially in a virtual machine.

The flushed access is an estimate of a cold memory access rather than a
guaranteed measurement of DRAM alone. It can be affected by hardware
prefetching, another CPU core, memory-controller state, operating-system
activity, and virtualization. Run it on a native Linux system for the most
stable results; WSL2 can produce large outliers.

## Timer overhead

Reading the timestamp counter is not free. This benchmark also uses memory
fences around each reading to prevent the CPU from moving the measured load
outside the timed region. Together, the timestamp reads and fences can take
far longer than an L1 cache hit.

Subtracting the measured timer overhead gives a rough estimate, but it also
amplifies small variations in instruction scheduling, interrupts, and CPU
frequency. If the timer and cached measurements round to the same median, the
adjusted result is zero even though the load did take time. A result in the
0 to 4 tick range should therefore not be interpreted as an exact L1 latency.

A more accurate way to measure short latencies is to time many dependent loads
as one operation and divide the total time by the number of loads. This
amortizes the fixed timer overhead while the dependency prevents the CPU from
executing the loads in parallel. Timer subtraction is more reliable for the
flushed load here because its latency is much larger than the overhead.

## Example results

Ten runs on an Intel Core Ultra 7 155U produced these median
timestamp-counter measurements. The adjusted time columns subtract the timer
overhead and use the measured 2.688 GHz TSC frequency:

| Run | Overhead (ticks) | Cached (ticks) | Flushed (ticks) | Adjusted cached (ticks) | Cached time (ns) | Adjusted flushed (ticks) | Flushed time (ns) |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 | 54 | 56 | 344 | 2 | 0.7 | 290 | 107.9 |
| 2 | 54 | 56 | 334 | 2 | 0.7 | 280 | 104.2 |
| 3 | 84 | 88 | 400 | 4 | 1.5 | 316 | 117.6 |
| 4 | 56 | 56 | 354 | 0 | 0.0 | 298 | 110.9 |
| 5 | 56 | 58 | 356 | 2 | 0.7 | 300 | 111.6 |
| 6 | 56 | 58 | 336 | 2 | 0.7 | 280 | 104.2 |
| 7 | 28 | 30 | 316 | 2 | 0.7 | 288 | 107.1 |
| 8 | 28 | 30 | 308 | 2 | 0.7 | 280 | 104.2 |
| 9 | 56 | 58 | 346 | 2 | 0.7 | 290 | 107.9 |
| 10 | 54 | 58 | 350 | 4 | 1.5 | 296 | 110.1 |

Across the ten runs, the adjusted cached result ranged from 0 to 4 ticks
(0.0 to 1.5 ns) with a median of 2 ticks (0.7 ns). The adjusted flushed result
ranged from 280 to 316 ticks (104.2 to 117.6 ns) with a median of 290 ticks
(107.9 ns). The timer overhead dominates the very short cached load, so its
adjusted time is too noisy for a precise latency measurement. The much larger
and consistent flushed result clearly shows the cost of a cold load.

These values are timestamp-counter ticks, not necessarily CPU core clock
cycles. The time estimates use `latency_ns = adjusted_ticks / 2.688 GHz`.
