#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include "hlslib/Stream.h"
using namespace hlslib;

constexpr int N = 32;
constexpr int M = 32;

void Stencil2D(float const memory_in[N * M], float memory_out[N * M]) {

  Stream<float> above_buffer(M);
  Stream<float> center_buffer(M);

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    above_buffer.Push(memory_in[i]);
  }

  for (int i = 0; i < M; ++i) {
    #pragma HLS PIPELINE
    center_buffer.Push(memory_in[M + i]);
  }

  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      #pragma HLS PIPELINE II=1

      const auto above = above_buffer.Pop();
      const auto center = center_buffer.Pop();
      const auto below = memory_in[(i + 1)*M + j];

      constexpr float factor = 0.3333;
      const auto average = factor * (above + center + below);

      above_buffer.Push(center);
      center_buffer.Push(below);

      memory_out[i * M + j] = average;
    }
  }

}









#ifdef USE_TIME

void Stencil2DTime(float const memory_in[N * M], float memory_out[N * M]) {

  Stream<float> above_buffer(M);
  Stream<float> center_buffer(M);

  for (int t = 0; t < T; ++t) {

    for (int i = 0; i < M; ++i) {
      #pragma HLS PIPELINE
      above_buffer.Push(memory_in[i]);
    }

    for (int i = 0; i < M; ++i) {
      #pragma HLS PIPELINE
      center_buffer.Push(memory_in[M + i]);
    }

    for (int i = 1; i < N - 1; ++i) {
      for (int j = 0; j < M; ++j) {
        #pragma HLS PIPELINE II=1

        const auto above = above_buffer.Pop();
        const auto center = center_buffer.Pop();
        const auto below = memory_in[(i + 1)*M + j];

        constexpr float factor = 0.3333;
        const auto average = factor * (above + center + below);

        above_buffer.Push(center);
        center_buffer.Push(below);

        memory_out[i * M + j] = average;
      }
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
#ifdef USE_TIME
  Stencil2DTime(in, out);
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
