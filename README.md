# cpu-cache-latency
A small script to check cpu cache vs ram access latency

```bash
make # builds the script
make run # builds and runs the script
```

The flushed access measurement is an estimate of RAM latency rather than a guaranteed measurement of DRAM alone. Although clflush removes the cache line from the processor’s caches, the result can still
be affected by hardware prefetching, another CPU core, memory-controller state, operating-system activity, and timer overhead. Therefore, ram_cycles is more accurately interpreted as the latency of a cold
or flushed memory access.
