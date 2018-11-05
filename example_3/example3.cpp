#include "example3.h" // Defines N and M

void Stencil2D(float const memory_in[N * M], float memory_out[N * M]) {

  Stream<float> above_buffer(M);
  Stream<float> center_buffer(M);

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    above_buffer.Push(memory_in[i]);
  }

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    center_buffer.Push(memory_in[M + i]);
  }

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto above = above_buffer.Pop();
      const auto center = center_buffer.Pop();
      const auto below = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above + center + below);

      // Avoid leftover elements in the buffer by not pushing on the last 
      // iteration
      if (i < N - 2) {
        above_buffer.Push(center);
        center_buffer.Push(below);
      }

      memory_out[i * M + j] = average;
    }
  }

}
