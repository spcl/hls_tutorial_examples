#include "hlslib/Stream.h"
using namespace hlslib;

constexpr int N = 32;
constexpr int M = 32;

void Stencil2D(float const memory_in[N * M], float memory_out[N * M]) {

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto above  = memory_in[(i - 1)*M + j];
      const auto center = memory_in[i*M + j];
      const auto below  = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above + center + below);

      memory_out[i * M + j] = average;
    }
  }

}














#ifdef USE_BUFFERED

void Stencil2DBuffered(float const memory_in[N * M], float memory_out[N * M]) {

  float above[M];
  float center[M];

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    above[i] = memory_in[i];
  }

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    center[i] = memory_in[M + i];
  }

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto below = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above[j] + center[j] + below);

      above[j] = center[j];
      center[j] = below;
      #pragma HLS DEPENDENCE variable=above inter false
      #pragma HLS DEPENDENCE variable=center inter false

      memory_out[i * M + j] = average;
    }
  }

}

#endif






#ifdef USE_STREAMS

void Stencil2DStreams(float const memory_in[N * M], float memory_out[N * M]) {

  Stream<float> above_buffer(M);
  Stream<float> center_buffer(M);

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE II=1
    above_buffer.write(memory_in[i]);
  }

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE II=1
    center_buffer.write(memory_in[M + i]);
  }

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto above = above_buffer.read();
      const auto center = center_buffer.read();
      const auto below = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above + center + below);

      above_buffer.write(center);
      center_buffer.write(below);

      memory_out[i * M + j] = average;
    }
  }

}

#endif









void Entry(float const *in, float *out) {
  #pragma HLS INTERFACE m_axi port=in bundle=gmem0 offset=slave 
  #pragma HLS INTERFACE m_axi port=out bundle=gmem1 offset=slave 
  #pragma HLS INTERFACE s_axilite port=in
  #pragma HLS INTERFACE s_axilite port=out
  #pragma HLS INTERFACE s_axilite port=return
#ifdef USE_BUFFERED
  Stencil2DBuffered(in, out);
#elif defined(USE_STREAMS)
  Stencil2DStreams(in, out);
#else
  Stencil2D(in, out);
#endif
}

#ifndef HLSLIB_SYNTHESIS
void Reference(float const in[], float out[]) {
  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      out[i * M + j] =
          0.3333 * (in[(i - 1) * M + j] + in[i * M + j] + in[(i + 1) * M + j]);
    }
  }
}

#include <algorithm>
#include <iostream>
#include <vector>
#include <random>

int main() {
  std::vector<float> in(N*M); 

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(in.begin(), in.end(), [&](float &i) { i = dist(rng); });

  std::vector<float> out_ref(N*M);
  std::vector<float> out_res(N*M);

  Entry(in.data(), out_res.data());
  Reference(in.data(), out_ref.data());

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j) {
      const auto diff = std::abs(out_ref[i*M + j] - out_res[i*M + j]);
      if (diff > 1e-4) {
        std::cerr << "Mismatch at (" << i << ", " << j
                  << "): " << out_res[i * M + j] << " (should be "
                  << out_ref[i * M + j] << ")." << std::endl;
        return 1;
      }
    }
  }

  return 0;
}
#endif
