## Example 2

This example is similar to example 1, but has changed the iteration space to two dimensions, and the memory access pattern to vertically adjacent elements. This means that the three elements required to output a result are no longer adjacent in memory, and must thus be buffered for longer; namely M cycles, where M is the width of the domain. 

1. Open `Example2.cpp` to see the required access pattern into `memory_in`.
2. Run `make` to synthesize the code. You will see `II=3` due to congestion on the memory port, similar to example 1.
3. Implement buffers to reduce the number of loads to 1 per iteration, but this time using arrays of size M for the above and center elements.
4. Initializing the arrays is now more than a single iteration: we will need two more loops to initialize the two buffers. Each of these should also be pipelined.
5. Run synthesis again, and see that the II has been reduced to 1 (see example implementation in `Example2_Buffered.cpp`).
6. As an alternative representation of the buffers, we can use _streams_ (FIFO buffers, provided as the `Stream<T>` class), as the required access pattern satisfies FIFO semantics. See `Example2_Streams.cpp` for how this can be done.
