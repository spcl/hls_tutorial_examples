#include "example2.h" // N and M are defined here

void Stencil2D(float const memory_in[N * M], float memory_out[N * M]) {

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto above  = memory_in[(i - 1)*M + j];
      const auto center = memory_in[i*M + j];
      const auto below  = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above + center + below);

      memory_out[i * M + j] = average;
    }
  }

}
