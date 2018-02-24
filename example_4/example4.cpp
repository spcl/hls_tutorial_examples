#include "hlslib/Simulation.h"
#include "hlslib/Stream.h"
using namespace hlslib;

constexpr int N = 1024;
constexpr int D = 8;

// Compute function/processing element
void Simple1DStencil(Stream<float> &stream_in, Stream<float> &stream_out,
                     int d) {
  // Registers
  float left;
  float center;

  for (int i = d; i < N - d; ++i) {
    #pragma HLS PIPELINE II=1
      
    const auto in = stream_in.Pop();

    // Saturation
    if (i == d) {
      left = in;
      continue;
    }
    if (i == d + 1) {
      center = in;
      continue;
    }

    // Read wavefront
    const auto right = in;

    // Compute
    constexpr float factor = 0.3333;
    const auto res = factor * (left + center + right); 

    // Update registers
    left = center;
    center = right; 

    // Write downstream 
    stream_out.Push(res);
  }

}






// Reads into the head of the pipeline
void ReadMemory(float const *in, Stream<float> &stream) {
  for (int i = 0; i < N; ++i) {
    #pragma HLS PIPELINE II=1
    stream.Push(in[i]);
  }
}

// Writes from the tail of the pipeline
void WriteMemory(Stream<float> &stream, float *out) {
  for (int i = D; i < N - D; ++i) {
    #pragma HLS PIPELINE II=1
    out[i] = stream.Pop();
  }
}

#ifdef HLSLIB_SYNTHESIS

// Top-level function
void Entry(float const *in, float *out) {
  #pragma HLS INTERFACE m_axi port=in  bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=out bundle=gmem1 offset=slave
  #pragma HLS INTERFACE s_axilite port=in
  #pragma HLS INTERFACE s_axilite port=out
  #pragma HLS INTERFACE s_axilite port=return

  #pragma HLS DATAFLOW

  Stream<float> pipe[D + 1];

  ReadMemory(in, pipe[0]);
  for (int d = 0; d < D; ++d) {
    #pragma HLS UNROLL
    Simple1DStencil(pipe[d], pipe[d + 1], d);
  }
  WriteMemory(pipe[D], out);
}
























#else

void Entry(float const *in, float *out) {
  #pragma HLS INTERFACE m_axi port=in bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=out bundle=gmem1 offset=slave
  #pragma HLS INTERFACE s_axilite port=in
  #pragma HLS INTERFACE s_axilite port=out
  #pragma HLS INTERFACE s_axilite port=return

  #pragma HLS DATAFLOW

  Stream<float> pipe[D + 1];

  HLSLIB_DATAFLOW_INIT();

  HLSLIB_DATAFLOW_FUNCTION(ReadMemory, in, pipe[0]);
  for (int d = 0; d < D; ++d) {
    #pragma HLS UNROLL
    HLSLIB_DATAFLOW_FUNCTION(Simple1DStencil, pipe[d], pipe[d + 1], d);
  }
  HLSLIB_DATAFLOW_FUNCTION(WriteMemory, pipe[D], out);

  HLSLIB_DATAFLOW_FINALIZE();
}

void Reference(float const in[], float out[]) {
  float tmp[N];
  std::copy(in, in + N, tmp);
  for (int d = 0; d < D; ++d) {
    auto read = (d % 2 == 0) ? tmp : out;
    auto write = (d % 2 == 0) ? out : tmp;
    for (int i = d; i < N - d; ++i) {
      write[i] = 0.3333 * (read[i - 1] + read[i] + read[i + 1]);
    }
  }
  if (D % 2 == 0) {
    std::copy(tmp, tmp + N, out);
  }
  for (int i = 0; i < D; ++i) {
    out[i] = 0;
  }
  for (int i = N - D; i < N; ++i) {
    out[i] = 0;
  }
}

#include <algorithm>
#include <random>
#include <vector>

int main() {
  std::vector<float> in(N);

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(in.begin(), in.end(), [&](float &i) { i = dist(rng); });

  std::vector<float> out_ref(N);
  std::vector<float> out_res(N);

  Entry(in.data(), out_res.data());
  Reference(in.data(), out_ref.data());

  for (int i = 0; i < N; ++i) {
    const auto diff = std::abs(out_ref[i] - out_res[i]);
    if (diff > 1e-4) {
      std::cerr << "Mismatch at " << i << ": " << out_res[i]
                << " (should be " << out_ref[i] << ")." << std::endl;
      // return 1;
    }
  }

  return 0;
}

#endif
