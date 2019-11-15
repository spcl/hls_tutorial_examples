#include "Example5.h" //Type definition is here

__kernel void MatrixMultiplication(__global TYPE_T const *restrict A,
                                   __global TYPE_T const *restrict B,
                                   __global TYPE_T *restrict C) {

  // With double precision, Intel Compiler is no longer able
  // to pipeline th loops with II = 1
  for (int n = 0; n < DIM_N; ++n) {
    for (int m = 0; m < DIM_M; ++m) {
      TYPE_T acc = 0;
      for (int k = 0; k < DIM_K; ++k) {
        acc += A[n * DIM_K + k] * B[k * DIM_M + m];
      }
      C[n * DIM_M + m] = acc;
    }
  }
}
