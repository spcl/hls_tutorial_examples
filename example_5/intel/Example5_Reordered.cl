#include "Example5.h"

__kernel void MatrixMultiplication(__global TYPE_T const *restrict A,
                                   __global TYPE_T const *restrict B,
                                   __global TYPE_T *restrict C) {
  for (int n = 0; n < DIM_N; ++n) {
    TYPE_T acc[DIM_M];  // Buffer an output row of C

    // If DIM_M is sufficiently larger, we can pipeline this loop
    // by ignoring dependencies over acc
    #pragma ivdep array(acc)
    for (int k = 0; k < DIM_K; ++k) {  // Collapsed dimension moved out

      const TYPE_T a =
          A[n * DIM_K + k];  // We only need to read A once per row of B

      for (int m = 0; m < DIM_M; ++m) {
        const TYPE_T prev = (k == 0) ? 0 : acc[m];  // Automatic "reset" during
        acc[m] = prev + a * B[k * DIM_M + m];       // first iteration of M-loop
      }
    }

    // Write out resulting row of C
    for (int m = 0; m < DIM_M; ++m) {
      C[n * DIM_M + m] = acc[m];
    }
  }
}
