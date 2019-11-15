#include "Example6.h"

void MatrixMultiplication(const float A[], const float B[], float C[]) {

  for (int n = 0; n < N / D; ++n) {

    float acc[D][M];
    #pragma HLS ARRAY_PARTITION variable=acc dim=1 complete

    for (int k = 0; k < K; ++k) {

      float a_buffer[D];
      for (int nd = 0; nd < D; ++nd) {
        #pragma HLS PIPELINE II=1
        a_buffer[nd] = A[n * D * K + nd * K + k];
      }

      for (int m = 0; m < M; ++m) {
        #pragma HLS PIPELINE II=1
        const auto b_val = B[k * M + m];
        for (int nd = 0; nd < D; ++nd) {
          #pragma HLS UNROLL
          const auto prev = (k > 0) ? acc[nd][m] : 0;
          acc[nd][m] = prev + a_buffer[nd] * b_val;
          #pragma HLS DEPENDENCE variable=acc inter false
        }
      }
    }

    for (int nd = 0; nd < D; ++nd) {
      for (int m = 0; m < M; ++m) {
        #pragma HLS LOOP_FLATTEN
        #pragma HLS PIPELINE II=1
        C[n * D * M + nd * M + m] = acc[nd][m];
      }
    }
  }
}
