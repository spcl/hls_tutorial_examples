constexpr int N = 1024;

void Simple1DStencil(float const *memory_in, float *memory_out) {

  // Registers
  float left = memory_in[0];
  float center = memory_in[1];

  for (int i = 1; i < N - 1; ++i) {
    #pragma HLS PIPELINE II=1

    const auto right  = memory_in[i + 1];

    constexpr float factor = 0.3333;
    const auto average = factor * (left + center + right);

    // Shift registers
    left = center;
    center = right;

    memory_out[i] = average;
  }

}

