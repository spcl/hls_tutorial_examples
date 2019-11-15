#include "Example6.h"

__kernel void MatrixMultiplication(__global float const *restrict A,
                                   __global float const *restrict B,
                                   __global float *restrict C) {

  for (int n = 0; n < N / D; ++n) {

    float acc[D][M];

    #pragma ivdep array(acc)
    for (int k = 0; k < K; ++k) {
      float a_buffer[D];
      for (int nd = 0; nd < D; ++nd) {
        a_buffer[nd] = A[n * D * K + nd * K + k];
      }
      for (int m = 0; m < M; ++m) {
        float b_val = B[k * M + m];
        #pragma unroll
        for (int nd = 0; nd < D; ++nd) {
          float prev = (k > 0) ? acc[nd][m] : 0;
          acc[nd][m] = prev + a_buffer[nd] * b_val;
        }
      }
    }

    for (int nd = 0; nd < D; ++nd) {
      for (int m = 0; m < M; ++m)
        C[n * D * M + nd * M + m] = acc[nd][m];
    }
  }
}
