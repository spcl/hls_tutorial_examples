#include "hlslib/Simulation.h"
#include "Example4.h"

// Compute function/processing element
void Simple1DStencil(Stream<float> &stream_in, Stream<float> &stream_out,
                     int d) {

  // Registers
  float left = stream_in.Pop();
  float center = stream_in.Pop();

  for (int i = d + 2; i < N - d; ++i) {
    #pragma HLS PIPELINE II=1
      
    const auto in = stream_in.Pop();

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
  for (int i = D; i < N - D; ++i) { // Take shrinkage into account
    #pragma HLS PIPELINE II=1
    out[i] = stream.Pop();
  }
}

void Entry(float const *in, float *out) {
  #pragma HLS INTERFACE m_axi port=in bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=out bundle=gmem1 offset=slave
  #pragma HLS INTERFACE s_axilite port=in
  #pragma HLS INTERFACE s_axilite port=out
  #pragma HLS INTERFACE s_axilite port=return

  #pragma HLS DATAFLOW

  Stream<float> pipe[D + 1];

  HLSLIB_DATAFLOW_INIT();

  // HLSLIB_DATAFLOW_FUNCTION emulates concurrent execution in hardware by
  // wrapping function calls in std::thread
 
  HLSLIB_DATAFLOW_FUNCTION(ReadMemory, in, pipe[0]);

  for (int d = 0; d < D; ++d) {
    #pragma HLS UNROLL // D-fold replication
    HLSLIB_DATAFLOW_FUNCTION(Simple1DStencil, pipe[d], pipe[d + 1], d);
  }
  HLSLIB_DATAFLOW_FUNCTION(WriteMemory, pipe[D], out);

  HLSLIB_DATAFLOW_FINALIZE();
}
