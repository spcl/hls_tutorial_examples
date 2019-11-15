void Simple1DStencil(float const *memory_in, float *memory_out);

// Top-level entry function, not relevant for this example
void Example1(float const *in, float *out) {
  #pragma HLS INTERFACE m_axi port=in bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=out bundle=gmem1 offset=slave
  #pragma HLS INTERFACE s_axilite port=in
  #pragma HLS INTERFACE s_axilite port=out
  #pragma HLS INTERFACE s_axilite port=return
  Simple1DStencil(in, out);
}
