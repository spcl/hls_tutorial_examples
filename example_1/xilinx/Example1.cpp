constexpr int N = 1024;

void Simple1DStencil(float const *memory_in, float *memory_out) {

  for (int i = 1; i < N - 1; ++i) {
    #pragma HLS PIPELINE II=1

    // We request 3 memory locations from DRAM here, which will be generated as
    // 3 separate memory requests.
    // In the best case scenario, this means that we can only perform the
    // computation every 3 cycles, when all data is available.
    //
    // TODO: Add buffers persistent across iterations by placing them outside
    // the loop, and use them to reduce the number of accesses from memory_in to
    // 1 per cycle.

    const auto left = memory_in[i - 1];
    const auto center = memory_in[i];
    const auto right = memory_in[i + 1];

    constexpr float factor = 0.3333;
    const auto average = factor * (left + center + right);

    memory_out[i] = average;
  }
}
