## Example 1

In this example, we look at a simple 1D stencil program, which needs to access three adjacent memory locations every cycle to compute a single output.

1. Open `Example1.cpp` and look over the code.
  - We have attempt to pipeline the loop with `II=1`.
  - In every iteration we access indices `i-1`, `i` and `i+1` of `memory_in`.
2. Run `make` to synthesize the design, and look at the output in the console.
  - You will get a warning that the tool was unable to enforce the pipeline pragma due to a dependence constraint between the three memory reads within a single iteration of the pipeline.
  - The resulting II will be 3, as only a single memory request from the interface can be resolved in a single cycle.
3. Modify the code to only have 1 load from `memory_in` per cycle using on-chip buffers. This can be done by defining variables outside the loop, such that they are persisent across loop iterations (example solution in `Example1_Pipelined.cpp`).
4. Run `make` again to observe that the initiation interval has now been reduced to 1, as only a single load is performed every iteration.
