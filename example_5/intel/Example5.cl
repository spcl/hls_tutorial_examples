#include "Example5.h" //Type definition is here

__kernel void MatrixMultiplication(__global TYPE_T const* restrict A, __global TYPE_T const* restrict B, __global TYPE_T* restrict C) {

  //With double precision, Intel Compiler is no longer able 
  //to pipeline th loops with II = 1
  for (int n = 0; n < N; ++n) {
    for (int m = 0; m < M; ++m) {
      TYPE_T acc = 0;
      for (int k = 0; k < K; ++k) {
        acc += A[n*K + k] * B[k*M + m];
      }
      C[n*M + m] = acc;
    }
  }

}
