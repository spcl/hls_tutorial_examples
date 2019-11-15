#include "Example2.h" // N and M are defined here

__kernel void Stencil2D(__global float *const restrict memory_in,
                        __global float *restrict memory_out) {

  float above[M];
  float center[M];

  // The first two rows are buffered in separate pipelines

  for (int i = 0; i < M; ++i) {
    above[i] = memory_in[i];
  }

  for (int i = 0; i < M; ++i) {
    center[i] = memory_in[M + i];
  }

  // The remaining rows can be streamed

#pragma ivdep array(above)
#pragma ivdep array(center)
  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {

      const float below = memory_in[(i + 1) * M + j];

      const float factor = 0.3333f;
      const float average = factor * (above[j] + center[j] + below);

      above[j] = center[j];
      center[j] = below;

      memory_out[i * M + j] = average;
    }
  }
}
