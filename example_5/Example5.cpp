#include "Example5.h"

void MatrixMultiplication(const float A[], const float B[], float C[]) {

  for (int n = 0; n < N; ++n) {
    for (int m = 0; m < M; ++m) {
      float acc = 0;
      for (int k = 0; k < K; ++k) {
        #pragma HLS PIPELINE II=1
        acc += A[n*K + k] * B[k*M + m];
      }
      C[n*M + m] = acc;
    }
  }

}
