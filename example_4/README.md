## Example 4

This example shows the factorization of different pipelines into separate _processing elements_, and how this can be exploited to scale up parallelism. 

`Example4.cpp` contains a simple stencil code similar to example 2, but has been modified to read from and write to streams instead of memory. We also expose the `Entry`-function, which is the top-level entity of the kernel, and will host all concurrent processing elements.
Each sub-function within `Entry` corresponds to a concurrently and independently scheduled processing element, communicating with other elements via streams (this is facilitated with the pragma `HLS DATAFLOW`).
To interface with the outside world, the function `ReadMemory` reads from a DRAM interface into a stream, and `WriteMemory` writes into DRAM from a stream.

Between the two memory functions, we have placed a _parametric_ number of computational processing elements. To achieve this, we put the function call in a loop which is _unrolled_, causing each "iteration" to be instantiated as hardware. We use the parameter `D` to adjust this (must be known at compile-time!).

1. Open `Example4.cpp`, and get a grasp of the structure causing the generation of multiple, concurrent processing elements.
2. Open `Example4.h` to see the values of `N`, `M` and `D`. 
3. Run `make` to synthesize the code, and open `report.rpt`.
  - The report will now contain pipelining information on all `D + 2` processing elements. 
  - Although `D + 2` elements are connected in sequence, the total runtime will only be approximately the runtime of the _slowest_ element (roughly `N*M`). **Note:** use the `Interval` metric, rather than the `Latency` metric, as the latter shows an inaccurate estimate as of Vivado HLS 2018.2.   
4. Try adjusting `N`, `M` and `D`. You will notice that increasing `D` has almost no influence on the total runtime (as this only increases the depth of the pipeline!), but increases performance, as each layer of means another stencil being applied in parallel in the pipeline. 
