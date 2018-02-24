#include "hlslib/Stream.h"

using namespace hlslib;

constexpr int N = 1024;

void InitiationInterval(float const *a_mem, float const *b_mem, float *c_mem) {
  for (int i = 0; i < N; ++i) {
    const auto a = a_mem[i];
    const auto b = b_mem[i];
    // ----
    #pragma HLS PIPELINE II=1
    float c = (a + b) * (a - b);
    // ----
    c_mem[i] = c;
  }
}
























void Entry(float const *a_mem, float const *b_mem, float *c_mem) {
  #pragma HLS INTERFACE m_axi port=a_mem offset=slave bundle=gmem0
  #pragma HLS INTERFACE m_axi port=b_mem offset=slave bundle=gmem1
  #pragma HLS INTERFACE m_axi port=c_mem offset=slave bundle=gmem2
  #pragma HLS INTERFACE s_axilite port=a_mem bundle=control 
  #pragma HLS INTERFACE s_axilite port=b_mem bundle=control 
  #pragma HLS INTERFACE s_axilite port=c_mem bundle=control 
  #pragma HLS INTERFACE s_axilite port=return
  #pragma HLS DATAFLOW
  InitiationInterval(a_mem, b_mem, c_mem);
}
