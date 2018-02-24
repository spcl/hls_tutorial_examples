#include "hlslib/DataPack.h"

constexpr int kSize = 1024;
constexpr int kUnrollN = 64;

void Unroll(const float A[], const float B[], float C[]) {

N:
  for (int n = 0; n < kSize / kUnrollN; ++n) {

    float acc[kUnrollN][kSize];
    #pragma HLS ARRAY_PARTITION variable=acc dim=1 complete

  M:
    for (int m = 0; m < kSize; ++m) {

      float a_buffer[kUnrollN];
    Buffer:
      for (int nu = 0; nu < kUnrollN; ++nu) {
        #pragma HLS PIPELINE II=1
        a_buffer[nu] = A[n*kSize*kUnrollN + nu*kSize + m];
      }

    P:
      for (int p = 0; p < kSize; ++p) {
        #pragma HLS PIPELINE II=1
        const auto b_val = B[m*kSize + p];
        for (int nu = 0; nu < kUnrollN; ++nu) {
          #pragma HLS UNROLL
          const auto prev = (m > 0) ? acc[nu][p] : 0;
          acc[nu][p] = prev + a_buffer[nu] * b_val;
          #pragma HLS DEPENDENCE variable=acc inter false
        }
      }
    }

  Write:
    for (int nu = 0; nu < kUnrollN; ++nu) {
      for (int p = 0; p < kSize; ++p) {
        #pragma HLS LOOP_FLATTEN
        #pragma HLS PIPELINE II=1
        C[n*kUnrollN*kSize + nu*kSize + p] = acc[nu][p];
      }
    }

  }

}

















#ifdef USE_VECTORIZED

constexpr int kWidth = 2;
using Vec_t = hlslib::DataPack<float, kWidth>;

void UnrollAndVectorize(const float A[], const Vec_t B[], Vec_t C[]) {

N:
  for (int n = 0; n < kSize / kUnrollN; ++n) {

    Vec_t acc[kUnrollN][kSize / kWidth];
    #pragma HLS ARRAY_PARTITION variable=acc dim=1 complete

  M:
    for (int m = 0; m < kSize; ++m) {

      float a_buffer[kUnrollN];
    Buffer:
      for (int nu = 0; nu < kUnrollN; ++nu) {
        #pragma HLS PIPELINE II=1
        a_buffer[nu] = A[n*kSize*kUnrollN + nu*kSize + m];
      }

    P:
      for (int p = 0; p < kSize / kWidth; ++p) {
        #pragma HLS PIPELINE II=1
        const auto b_val = B[m*(kSize / kWidth) + p];
        for (int nu = 0; nu < kUnrollN; ++nu) {
          #pragma HLS UNROLL
          const Vec_t prev = (m > 0) ? acc[nu][p] : 0.;
          const auto a = a_buffer[nu];
          Vec_t res;
          for (int w = 0; w < kWidth; ++w) {
            #pragma HLS UNROLL
            res[w] = prev[w] + a * b_val[w];
          }
          acc[nu][p] = res;
          #pragma HLS DEPENDENCE variable=acc inter false
        }
      }
    }

  Write:
    for (int nu = 0; nu < kUnrollN; ++nu) {
      for (int p = 0; p < kSize / kWidth; ++p) {
        #pragma HLS LOOP_FLATTEN
        #pragma HLS PIPELINE II=1
        C[n*kUnrollN*(kSize / kWidth) + nu*(kSize / kWidth) + p] = acc[nu][p];
      }
    }

  }

}

#endif































#ifndef USE_VECTORIZED

void Entry(float const A[], float const B[], float C[]) {
  #pragma HLS INTERFACE m_axi port=A bundle=gmem0 offset=slave 
  #pragma HLS INTERFACE m_axi port=B bundle=gmem1 offset=slave 
  #pragma HLS INTERFACE m_axi port=C bundle=gmem2 offset=slave 
  #pragma HLS INTERFACE s_axilite port=A bundle=control
  #pragma HLS INTERFACE s_axilite port=B bundle=control
  #pragma HLS INTERFACE s_axilite port=C bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  Unroll(A, B, C);
}

#else

void EntryVectorized(float const A[], Vec_t const B[], Vec_t C[]) {
  #pragma HLS INTERFACE m_axi port=A bundle=gmem0 offset=slave 
  #pragma HLS INTERFACE m_axi port=B bundle=gmem1 offset=slave 
  #pragma HLS INTERFACE m_axi port=C bundle=gmem2 offset=slave 
  #pragma HLS INTERFACE s_axilite port=A bundle=control
  #pragma HLS INTERFACE s_axilite port=B bundle=control
  #pragma HLS INTERFACE s_axilite port=C bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  UnrollAndVectorize(A, B, C);
}
#endif

// ---------------------------------------------------------------------------
// Host side code
// ---------------------------------------------------------------------------

#ifndef HLSLIB_SYNTHESIS
#include <random>
#include <vector>
#include <iostream>

void Reference(float const a[], float const b[], float c[]) {
  for (int n = 0; n < kSize; ++n) {
    for (int p = 0; p < kSize; ++p) {
      c[n * kSize + p] = 0;
      for (int m = 0; m < kSize; ++m) {
        c[n * kSize + p] += a[n * kSize + m] * b[m * kSize + p];
      }
    }
  }
}

int main() {

  std::vector<float> a(kSize*kSize);
  std::vector<float> b(kSize*kSize);
  std::vector<float> cReference(kSize*kSize, 0);
  std::vector<float> cTiled(kSize*kSize, 0);

  std::random_device rd;
  std::default_random_engine rng(rd());
  std::uniform_real_distribution<float> dist;
  std::for_each(a.begin(), a.end(), [&](float &i) { i = dist(rng); });
  std::for_each(b.begin(), b.end(), [&](float &i) { i = dist(rng); });

  // Reference implementation for comparing the result
  Reference(a.data(), b.data(), cReference.data());

  // Run hardware simulation
#ifndef USE_VECTORIZED
  Entry(a.data(), &b[0], &cTiled[0]);
#else
  EntryVectorized(a.data(), reinterpret_cast<Vec_t const *>(&b[0]),
                  reinterpret_cast<Vec_t *>(&cTiled[0]));
#endif

  // Verify correctness
  for (int i = 0; i < kSize * kSize; ++i) {
    const auto diff = std::abs(cReference[i] - cTiled[i]);
    if (diff >= 1e-3) {
      std::cout << "Mismatch at (" << i / kSize << ", " << i % kSize
                << "): " << cTiled[i] << " (should be " << cReference[i]
                << ").\n";
      return 1;
    }
  }

  return 0;
}
#endif
