#include "Example6.h"

__kernel void MatrixMultiplication(__global float const *restrict A,
                                   __global float const *restrict B,
                                   __global float *restrict C) {
  for (int n = 0; n < DIM_N / PAR; ++n) {
    float acc[PAR][DIM_M];

    #pragma ivdep array(acc)
    for (int k = 0; k < DIM_K; ++k) {
      float a_buffer[PAR];
      for (int nd = 0; nd < PAR; ++nd) {
        a_buffer[nd] = A[n * PAR * DIM_K + nd * DIM_K + k];
      }
      for (int m = 0; m < DIM_M; ++m) {
        float b_val = B[k * DIM_M + m];
        #pragma unroll
        for (int nd = 0; nd < PAR; ++nd) {
          float prev = (k > 0) ? acc[nd][m] : 0;
          acc[nd][m] = prev + a_buffer[nd] * b_val;
        }
      }
    }

    for (int nd = 0; nd < PAR; ++nd) {
      for (int m = 0; m < DIM_M; ++m)
        C[n * PAR * DIM_M + nd * DIM_M + m] = acc[nd][m];
    }
  }
}
