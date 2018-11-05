#include "Example2.h" // N and M are defined here

void Stencil2D(float const memory_in[N * M], float memory_out[N * M]) {

  Stream<float> above_buffer(M);
  Stream<float> center_buffer(M);

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE II=1
    above_buffer.write(memory_in[i]);
  }

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE II=1
    center_buffer.write(memory_in[M + i]);
  }

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto above = above_buffer.read();
      const auto center = center_buffer.read();
      const auto below = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above + center + below);

      above_buffer.write(center);
      center_buffer.write(below);

      memory_out[i * M + j] = average;
    }
  }

}
