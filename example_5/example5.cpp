#include "hlslib/DataPack.h"

constexpr int N = 32;
constexpr int M = N;
constexpr int P = N;

void NaiveGEMM(const float A[], const float B[], float C[]) {

  for (int n = 0; n < N; ++n) {
    for (int p = 0; p < P; ++p) {
      float acc = 0;
      for (int m = 0; m < M; ++m) {
        #pragma HLS PIPELINE II=1
        acc += A[n*M + m] * B[m*P + p];
      }
      C[n*P + p] = acc;
    }
  }

}























void SwapIndices(const float A[], const float B[], float C[]) {

  for (int n = 0; n < N; ++n) {
    float acc[P];
    for (int m = 0; m < M; ++m) {
      const auto a = A[n*M + m];
      for (int p = 0; p < P; ++p) {
        #pragma HLS PIPELINE II=1
        const float prev = (m == 0) ? 0 : acc[p];
        acc[p] = prev + a * B[m*P + p];
        #pragma HLS DEPENDENCE variable=acc inter false
      }
    }
    for (int p = 0; p < N; ++p) {
      #pragma HLS PIPELINE II=1
      C[n*P + p] = acc[p];
    }
  }

}































// ---------------------------------------------------------------------------
// Entry function 
// ---------------------------------------------------------------------------

void Entry(float const A[], float const B[], float C[]) {
  #pragma HLS INTERFACE m_axi port=A bundle=gmem0 offset=slave 
  #pragma HLS INTERFACE m_axi port=B bundle=gmem1 offset=slave 
  #pragma HLS INTERFACE m_axi port=C bundle=gmem2 offset=slave 
  #pragma HLS INTERFACE s_axilite port=A bundle=control
  #pragma HLS INTERFACE s_axilite port=B bundle=control
  #pragma HLS INTERFACE s_axilite port=C bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control
#ifndef USE_REORDERED
  NaiveGEMM(A, B, C);
#else
  SwapIndices(A, B, C);
#endif
}

// ---------------------------------------------------------------------------
// Host side code
// ---------------------------------------------------------------------------

#ifndef HLSLIB_SYNTHESIS
#include <random>
#include <vector>
#include <iostream>

void Reference(float const a[], float const b[], float c[]) {
  for (int n = 0; n < N; ++n) {
    for (int p = 0; p < P; ++p) {
      c[n * P + p] = 0;
      for (int m = 0; m < M; ++m) {
        c[n * P + p] += a[n * M + m] * b[m * P + p];
      }
    }
  }
}

int main() {

  std::vector<float> a(N * M);
  std::vector<float> b(M * P);
  std::vector<float> cReference(N * P, 0);
  std::vector<float> cTiled(N * P, 0);

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(a.begin(), a.end(), [&](float &i) { i = dist(rng); });
  std::for_each(b.begin(), b.end(), [&](float &i) { i = dist(rng); });

  // Reference implementation for comparing the result
  Reference(a.data(), b.data(), cTiled.data());

  // Run hardware simulation
  Entry(a.data(), &b[0], &cReference[0]);

  // Verify correctness
  for (int i = 0; i < N * P; ++i) {
    const auto diff = std::abs(cReference[i] - cTiled[i]);
    if (diff >= 1e-3) {
      std::cout << "Mismatch at (" << i / P << ", " << i % P 
                << "): " << cTiled[i] << " (should be " << cReference[i]
                << ").\n";
      return 1;
    }
  }

  return 0;
}
#endif
