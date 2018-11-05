#include "Example2.h" // N and M are defined here

void Stencil2D(float const memory_in[N * M], float memory_out[N * M]) {

  float above[M];
  float center[M];

  // The first two rows are buffered in separate pipelines

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    above[i] = memory_in[i];
  }

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    center[i] = memory_in[M + i];
  }

  // The remaining rows can be streamed

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto below = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above[j] + center[j] + below);

      above[j] = center[j];
      center[j] = below;
      #pragma HLS DEPENDENCE variable=above false
      #pragma HLS DEPENDENCE variable=center false

      memory_out[i * M + j] = average;
    }
  }

}
