#include "Example5.h"

__kernel void MatrixMultiplication(__global TYPE_T const *restrict A,
                                   __global TYPE_T const *restrict B,
                                   __global TYPE_T *restrict C) {

  for (int n = 0; n < N; ++n) {

    TYPE_T acc[M]; // Buffer an output row of C

// If M is sufficiently larger, we can pipeline this loop
// by ignoring dependencies over acc
#pragma ivdep array(acc)
    for (int k = 0; k < K; ++k) { // Collapsed dimension moved out

      const TYPE_T a = A[n * K + k]; // We only need to read A once per row of B

      for (int m = 0; m < M; ++m) {
        const TYPE_T prev = (k == 0) ? 0 : acc[m]; // Automatic "reset" during
        acc[m] = prev + a * B[k * M + m];          // first iteration of M-loop
      }
    }

    // Write out resulting row of C
    for (int m = 0; m < M; ++m) {
      C[n * M + m] = acc[m];
    }
  }
}