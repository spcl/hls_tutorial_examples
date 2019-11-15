#include "Example7.h"
#include "hlslib/xilinx/Simulation.h"
#include "hlslib/xilinx/Stream.h"

using hlslib::Stream;

void ProcessingElement(Stream<float> &a_in, Stream<float> &a_out,
                       Stream<Vec_t> &b_in, Stream<Vec_t> &b_out,
                       Stream<Vec_t> &c_in, Stream<Vec_t> &c_out, int d) {

  // Loop over tiles of C
  for (int bn = 0; bn < N / D; ++bn) {
    for (int bm = 0; bm < M / (TM * W); ++bm) {
      #pragma HLS LOOP_FLATTEN

      Vec_t c_buffer[TM]; // Local result buffer

      // Loop over collapsed dimension
      for (int k = 0; k < K; ++k) {

        float a_buffer; // Local buffer of A

        // Buffer own value and forward columns to saturate all PEs
        for (int nd = 0; nd < D - d; ++nd) {
          #pragma HLS PIPELINE II=1
          a_buffer = a_in.Pop();
          if (nd < D - d - 1) {
            a_out.Push(a_buffer);
          }
        }

        // Stream row of B and apply it to the stored value of A
        for (int tm = 0; tm < TM; ++tm) {
          #pragma HLS PIPELINE II=1
          const auto b_read = b_in.Pop();
          const auto c_prev = (k > 0) ? c_buffer[tm] : Vec_t(0.);
          c_buffer[tm] = c_prev + a_buffer * b_read;
          #pragma HLS DEPENDENCE variable=c_buffer false
          // Forward to subsequent PE, if any
          if (d < D - 1) {
            b_out.Push(b_read);
          }
        }

      } // End loop over K

      // Write out result block
      for (int tn = 0; tn < d + 1; ++tn) { // Different for each PE
        for (int tm = 0; tm < TM; ++tm) {
          #pragma HLS PIPELINE II=1
          #pragma HLS LOOP_FLATTEN
          Vec_t c_val;
          if (d > 0 && tn > 0) {
            c_val = c_in.Pop();
          } else {
            c_val = c_buffer[tm];
          }
          c_out.Push(c_val);
        }
      }
    }
  }
}

inline int GetAIndex(int bn, int tn, int m) {
  #pragma HLS INLINE
  return bn * D * K + tn * K + m;
}

inline int GetBIndex(int bm, int k, int tm) {
  #pragma HLS INLINE
  return k * (M / W) + bm * TM + tm;
}

inline int GetCIndex(int bn, int bm, int tn, int tm) {
  #pragma HLS INLINE
  return bn * D * (M / W) + tn * (M / W) + bm * TM + tm;
}

void ReadA(float const a[], Stream<float> &a_pipe) {
  for (int bn = 0; bn < N / D; ++bn) {
    for (int bm = 0; bm < M / (TM * W); ++bm) {
      for (int k = 0; k < K; ++k) {
        for (int tn = 0; tn < D; ++tn) {
          #pragma HLS LOOP_FLATTEN
          #pragma HLS PIPELINE II=1
          const auto read = a[GetAIndex(bn, tn, k)];
          a_pipe.Push(read);
        }
      }
    }
  }
}

void ReadB(Vec_t const b[], Stream<Vec_t> &b_pipe) {
  for (int bn = 0; bn < N / D; ++bn) {
    for (int bm = 0; bm < M / (TM * W); ++bm) {
      for (int k = 0; k < K; ++k) {
        for (int tm = 0; tm < TM; ++tm) {
          #pragma HLS LOOP_FLATTEN
          #pragma HLS PIPELINE II=1
          b_pipe.Push(b[GetBIndex(bm, k, tm)]);
        }
      }
    }
  }
}

void WriteC(Stream<Vec_t> &c_pipe, Vec_t c[]) {
  for (int bn = 0; bn < N / D; ++bn) {
    for (int bm = 0; bm < M / (TM * W); ++bm) {
      for (int tn = 0; tn < D; ++tn) {
        for (int tm = 0; tm < TM; ++tm) {
          #pragma HLS LOOP_FLATTEN
          #pragma HLS PIPELINE II=1
          c[GetCIndex(bn, bm, tn, tm)] = c_pipe.Pop();
        }
      }
    }
  }
}

void Example7(float const a[], Vec_t const b[], Vec_t c[]) {

  #pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
  #pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
  #pragma HLS INTERFACE m_axi port=c offset=slave bundle=gmem2
  #pragma HLS INTERFACE s_axilite port=a bundle=control
  #pragma HLS INTERFACE s_axilite port=b bundle=control
  #pragma HLS INTERFACE s_axilite port=c bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  
  #pragma HLS DATAFLOW

  Stream<float> a_pipes[D + 1];
  Stream<Vec_t> b_pipes[D + 1];
  Stream<Vec_t> c_pipes[D + 1];

  HLSLIB_DATAFLOW_INIT();
  HLSLIB_DATAFLOW_FUNCTION(ReadA, a, a_pipes[0]);
  HLSLIB_DATAFLOW_FUNCTION(ReadB, b, b_pipes[0]);
  for (int d = 0; d < D; ++d) {
    #pragma HLS UNROLL
    HLSLIB_DATAFLOW_FUNCTION(ProcessingElement, a_pipes[d], a_pipes[d + 1],
                             b_pipes[d], b_pipes[d + 1], c_pipes[d],
                             c_pipes[d + 1], d);
  }
  HLSLIB_DATAFLOW_FUNCTION(WriteC, c_pipes[D], c);
  HLSLIB_DATAFLOW_FINALIZE();
}
