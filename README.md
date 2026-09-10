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
