#include "Example6.h"

void MatrixMultiplication(const float A[], const Vec_t B[], Vec_t C[]) {

  for (int n = 0; n < N / D; ++n) {

    Vec_t acc[D][M / W];
    #pragma HLS ARRAY_PARTITION variable=acc dim=1 complete

    for (int k = 0; k < K; ++k) {

      float a_buffer[D];
      for (int nd = 0; nd < D; ++nd) {
        #pragma HLS PIPELINE II=1
        a_buffer[nd] = A[n * D * K + nd * K + k];
      }

      for (int m = 0; m < M / W; ++m) {
        #pragma HLS PIPELINE II=1
        const auto b_val = B[k * (M / W) + m];
        for (int nd = 0; nd < D; ++nd) {
          #pragma HLS UNROLL
          const auto prev = (k > 0) ? acc[nd][m] : Vec_t{static_cast<float>(0)};
          acc[nd][m] = prev + a_buffer[nd] * b_val;
        }
      }
    }

    for (int nd = 0; nd < D; ++nd) {
      for (int m = 0; m < M / W; ++m) {
        #pragma HLS LOOP_FLATTEN
        #pragma HLS PIPELINE II=1
        C[n * D * (M / W) + nd * (M / W) + m] = acc[nd][m];
      }
    }
  }
}
