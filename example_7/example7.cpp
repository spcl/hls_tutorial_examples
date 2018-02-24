#include "hlslib/DataPack.h"
#include "hlslib/Simulation.h"
#include "hlslib/Stream.h"
using namespace hlslib;

constexpr int kSize = 64;
constexpr int kWidth = 4; 
constexpr int kTileSizeN = 8;
constexpr int kTileSizeP = 16;
static_assert(kSize % kTileSizeN == 0, "Size must be divisable by tile size");
static_assert(kSize % (kTileSizeP * kWidth) == 0,
              "Size must be divisable by tile size");

using Vec_t = hlslib::DataPack<float, kWidth>;

// ---------------------------------------------------------------------------
// Processing element 
// ---------------------------------------------------------------------------

void Compute(Stream<float> &a_in, Stream<float> &a_out,
             Stream<Vec_t> &b_in, Stream<Vec_t> &b_out,
             Stream<Vec_t> &c_in, Stream<Vec_t> &c_out,
             int d) {

  // Loop over tiles of C
  Block_N: for (int bn = 0; bn < kSize / kTileSizeN; ++bn) {
    Block_P: for (int bp = 0; bp < kSize / (kTileSizeP * kWidth); ++bp) {
      #pragma HLS LOOP_FLATTEN

      Vec_t c_buffer[kTileSizeP]; // Local result buffer

      // Loop over collapsed dimension
      M: for (int m = 0; m < kSize; ++m) {

        float a_buffer; // Local buffer of A

        // Buffer own value and forward columns to saturate all PEs
        Buffer_A: for (int n = 0; n < kTileSizeN - d; ++n) {
          #pragma HLS PIPELINE
          a_buffer = ReadBlocking(a_in); 
          if (n < kTileSizeN - d - 1) {
            WriteBlocking(a_out, a_buffer); 
          }
        }

        // Stream row of B and apply it to the stored value of A 
        StreamB: for (int tp = 0; tp < kTileSizeP; ++tp) {
          #pragma HLS PIPELINE
          const auto b_read = ReadBlocking(b_in);
          const auto c_prev = (m > 0) ? c_buffer[tp] : Vec_t(0.0);
          c_buffer[tp] = c_prev + a_buffer * b_read;
          #pragma HLS DEPENDENCE inter variable=c_buffer false
          if (d < kTileSizeN - 1) {
            WriteBlocking(b_out, b_read);
          }
        }

      } // End loop over M

      // Write out result block
      WriteOut_N: for (int tn = 0; tn < d + 1; ++tn) {
        WriteOut_P: for (int tp = 0; tp < kTileSizeP; ++tp) {
          #pragma HLS PIPELINE
          #pragma HLS LOOP_FLATTEN
          Vec_t c_val;
          if (d > 0 && tn > 0) {
            c_val = c_in.ReadBlocking();
          } else {
            c_val = c_buffer[tp];
          }
          WriteBlocking(c_out, c_val);
        }
      }

    }
  }
}

// ---------------------------------------------------------------------------
// Memory functions 
// ---------------------------------------------------------------------------

inline int GetAIndex(int bn, int tn, int m) {
  #pragma HLS INLINE
  return bn * kTileSizeN * kSize + tn * kSize + m;
}

inline int GetBIndex(int bp, int m, int tp) {
  #pragma HLS INLINE
  return m * (kSize / kWidth) + bp * kTileSizeP + tp;
}

inline int GetCIndex(int bn, int bp, int tn, int tp) {
  #pragma HLS INLINE
  return bn * (kSize / kWidth) * kTileSizeN + tn * (kSize / kWidth) +
         bp * kTileSizeP + tp;
}

void ReadA(float const a[], Stream<float> &a_pipe) {
ReadA_Block_N:
  for (int bn = 0; bn < kSize / kTileSizeN; ++bn) {
  ReadA_Block_P:
    for (int bp = 0; bp < kSize / (kTileSizeP * kWidth); ++bp) {
    ReadA_M:
      for (int m = 0; m < kSize; ++m) {
      ReadA_N:
        for (int tn = 0; tn < kTileSizeN; ++tn) {
          #pragma HLS LOOP_FLATTEN
          #pragma HLS PIPELINE
          const auto read = a[GetAIndex(bn, tn, m)];
          WriteBlocking(a_pipe, read);
        }
      }
    }
  }
}

void ReadB(Vec_t const b[], Stream<Vec_t> &b_pipe) {
ReadBMemory_Block_N:
  for (int bn = 0; bn < kSize / kTileSizeN; ++bn) {
  ReadBMemory_Block_P:
    for (int bp = 0; bp < kSize / (kTileSizeP * kWidth); ++bp) {
    ReadBMemory_M:
      for (int m = 0; m < kSize; ++m) {
      ReadBMemory_P:
        for (int tp = 0; tp < kTileSizeP; ++tp) {
          #pragma HLS LOOP_FLATTEN
          #pragma HLS PIPELINE
          WriteBlocking(b_pipe, b[GetBIndex(bp, m, tp)]);
        }
      }
    }
  }

}

void WriteC(Stream<Vec_t> &c_pipe, Vec_t c[]) {
WriteCMemory_Block_N:
  for (int bn = 0; bn < kSize / kTileSizeN; ++bn) {
  WriteCMemory_Block_P:
    for (int bp = 0; bp < kSize / (kTileSizeP * kWidth); ++bp) {
    WriteCMemory_N:
      for (int tn = 0; tn < kTileSizeN; ++tn) {
      WriteCMemory_P:
        for (int tp = 0; tp < kTileSizeP; ++tp) {
          #pragma HLS LOOP_FLATTEN
          #pragma HLS PIPELINE
          c[GetCIndex(bn, bp, tn, tp)] = ReadBlocking(c_pipe);
        }
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Entry function 
// ---------------------------------------------------------------------------

void Entry(float const a[], Vec_t const b[], Vec_t c[]) {

  #pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
  #pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
  #pragma HLS INTERFACE m_axi port=c offset=slave bundle=gmem2
  #pragma HLS INTERFACE s_axilite port=a bundle=control
  #pragma HLS INTERFACE s_axilite port=b bundle=control
  #pragma HLS INTERFACE s_axilite port=c bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control

  #pragma HLS DATAFLOW

  Stream<float> a_pipes[kTileSizeN + 1];
  Stream<Vec_t> b_pipes[kTileSizeN + 1];
  Stream<Vec_t> c_pipes[kTileSizeN + 1];

  HLSLIB_DATAFLOW_INIT();
  HLSLIB_DATAFLOW_FUNCTION(ReadA, a, a_pipes[0]); 
  HLSLIB_DATAFLOW_FUNCTION(ReadB, b, b_pipes[0]); 
  for (int d = 0; d < kTileSizeN; ++d) {
    #pragma HLS UNROLL
    HLSLIB_DATAFLOW_FUNCTION(Compute, a_pipes[d], a_pipes[d + 1], b_pipes[d],
                             b_pipes[d + 1], c_pipes[d], c_pipes[d + 1], d);
  }
  HLSLIB_DATAFLOW_FUNCTION(WriteC, c_pipes[kTileSizeN], c); 
  HLSLIB_DATAFLOW_FINALIZE();
}

// ---------------------------------------------------------------------------
// Host side code
// ---------------------------------------------------------------------------

#ifndef HLSLIB_SYNTHESIS
#include "hlslib/Simulation.h"
#include <algorithm>
#include <iostream>
#include <random>

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

// ---------------------------------------------------------------------------
// Testbench 
// ---------------------------------------------------------------------------

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
  Reference(a.data(), b.data(), cTiled.data());

  // Run hardware simulation
  Entry(a.data(), reinterpret_cast<Vec_t const *>(&b[0]),
        reinterpret_cast<Vec_t *>(&cReference[0]));

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
