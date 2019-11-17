## Example 3

This example shows the influence of pipeline latencies on the total cycle count for nested loops.

1. Add a loop over a third dimension around all three inner loops (`for (int t = 0; t < T; ++t)`).
2. Open `xilinx/Example3.h` and observe what the values of `N`, `M`, and `T` are. 
3. Run `make synthesize_example` to synthesize, and look at the cycle count in `report_example3.rpt`.
4. Try modifying `N`, `M` and `T` to be either:
  - `N` and `M` small, `T` large 
  - `N` and `M` large, `T` small
5. You should observe that for small values of `N` and `M`, the total cycle count will be considerably higher than the "ideal" situation of `N*M*T`, whereas for large `N` and `M`, it will be close to the ideal value.
