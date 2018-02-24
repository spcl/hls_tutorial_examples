#include "hlslib/Stream.h"

using namespace hlslib;

constexpr int N = 1024;

void Simple1DStencil(float const *memory_in, float *memory_out) {

  for (int i = 1; i < N - 1; ++i) {
    #pragma HLS PIPELINE II=1

    const auto left   = memory_in[i - 1];
    const auto center = memory_in[i];
    const auto right  = memory_in[i + 1];

    constexpr float factor = 0.3333;
    const auto average = factor * (left + center + right);

    memory_out[i] = average;
  }

}















void Simple1DStencilBuffered(float const *memory_in, float *memory_out) {

  // Registers
  float left = memory_in[0];
  float center = memory_in[1];

  for (int i = 1; i < N - 1; ++i) {
    #pragma HLS PIPELINE II=1

    const auto right  = memory_in[i + 1];

    constexpr float factor = 0.3333;
    const auto average = factor * (left + center + right);

    // Shift registers
    left = center;
    center = right;

    memory_out[i] = average;
  }

}













void Entry(float const *in, float *out) {
  #pragma HLS INTERFACE m_axi port=in bundle=gmem0 offset=slave 
  #pragma HLS INTERFACE m_axi port=out bundle=gmem1 offset=slave 
  #pragma HLS INTERFACE s_axilite port=in
  #pragma HLS INTERFACE s_axilite port=out
  #pragma HLS INTERFACE s_axilite port=return
  Simple1DStencil(in, out);
  // Simple1DStencilBuffered(in, out);
}
