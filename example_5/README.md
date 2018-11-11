## Example 5

`Example5.cpp` contains a naive triple nested loop implementation of matrix multiplication.
In this implementation, an inter-iteration ("loop-carried") dependency arises because of the accumulation into `acc`: Because every iteration of the inner loop depends on the result of the previous iteration, this accumulation cannot be pipelined to more than the number of cycles it takes to compute a single addition. 
We will see how we can eliminate the loop-carried dependency and improve the memory access pattern by doing loop reordering.

1. Synthesize the example with `make`. Pipelining to `II=1` will not succeed because of the inter-iteration dependency on the accumulation variable. 
2. We will mend this by reordering the `K` and `M` loops:
  - To do this change, the accumulation buffer will change from a single value outside the `K`-loop to an array outside both the `K` and `M` loops.
  - In the inner loop, we update the accumulation buffer at position `m`. This means that the same position will only be updated every `M` cycles.
  - Writing out the result to `C` now becomes a loop, as we have computed a full row before results are read.
  - (An example implementation is given in `Example5_Reordered.cpp`)
3. Run `make` again. The inner loop should now be pipelined.
  - The accumulation buffer will now take up more storage (proportional to `M`).
  - An extra pipeline is introduced, introducing an extra drain phase (writing back `C`), which would negatively affect performance for small values of `N` and `M`.
4. Although we increased the accumulation buffer size to `M`, it is only necessary to restrict the number of cycles to be larger than the number of cycles taken to perform an addition in order to overcome the inter-iteration dependency. To reduce memory usage, we could tile the `M`-dimension. 
