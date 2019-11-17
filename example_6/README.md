## Example 6

This example plays with scaling up parallelism in a matrix multiplication example.
We add two degrees of parallelism: horizontal vector parallelism, which consumes bandwidth, and pipeline parallelism, which consumes buffer space.
In `xilinx/Example6.cpp`, rather  than loading a single value of `a` before the inner `M`-loop, we load a higher (parametric) number of elements, allowing us to apply each of them to every value of `B` loaded.
In `xilinx/Example6_Vectorized.cpp`, we additionally add vectorization, by replacing the primitive data types with vector types, and adjusting the loop bounds accordingly.
The two files can be synthesized with `make synthesize_example6` and `make synthesize_example6_vectorized`, respectively.

1. Observe the values of `N`, `M`, `K`, `D` (depth of `A`-buffer) and `W` (vector width) in `xilinx/Example6.h`.
2. Predict what the total cycle count should be based on these values, then run synthesis to see if this matches expectations.
3. Try increasing `D` and see how it affects the cycle count.
4. Reset `D`, then try increasing `W` and see how it affects the cycle count.
5. Try increasing both `D` and `W`, and see how it affects the cycle count.
6. You will observe that both `D` and `W` come with significant diminishing returns. To offset this, try increasing `N`, `M`, and `P`. This will decrease the relative effect of the diminishing returns, as the latency of the three pipelines is relatively less than the number of iterations. 
