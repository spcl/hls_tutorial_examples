#include "Example5.h"

void MatrixMultiplication(const float A[], const float B[], float C[]) {

  for (int n = 0; n < N; ++n) {

    float acc[M]; // Buffer an output row of C

    for (int k = 0; k < K; ++k) { // Collapsed dimension moved out

      const auto a = A[n*K + k]; // We only need to read A once per row of B

      for (int m = 0; m < M; ++m) {
        #pragma HLS PIPELINE II=1
        const float prev = (m == 0) ? 0 : acc[k]; // Automatic "reset" during
        acc[k] = prev + a * B[k*M + m];           // first iteration of M-loop
        #pragma HLS DEPENDENCE variable=acc inter false
      }

    }

    // Write out resulting row of C
    for (int m = 0; m < M; ++m) {
      #pragma HLS PIPELINE II=1
      C[n*M + m] = acc[m];
    }

  }

}
