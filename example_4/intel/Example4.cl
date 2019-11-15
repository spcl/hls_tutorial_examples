#pragma OPENCL EXTENSION cl_intel_channels : enable
#include "Example4.h"

// Channels
channel float systolicChannels[DEPTH + 1] __attribute__((depth(1)));

__kernel void ReadMemory(__global float *const in) {
  for (int i = 0; i < DIM_N; ++i)
    write_channel_intel(systolicChannels[0], in[i]);
}

__kernel void WriteMemory(__global float *out) {
  for (int i = DEPTH; i < DIM_N - DEPTH; ++i)
    out[i] = read_channel_intel(systolicChannels[DEPTH]);
}

__attribute__((max_global_work_dim(0))) __attribute__((autorun))
__attribute__((num_compute_units(DEPTH, 1))) __kernel void
Simple1DStencil() {
  int d = get_compute_id(0);

  // registers
  float prev[2]; // left and center elements
  for (int i = d; i < DIM_N - d; ++i) {
    float in = read_channel_intel(systolicChannels[d]);
    if (i < d + 2)
      prev[i - d] = in;
    else {

      // Read wavefront
      float left = prev[0];
      float center = prev[1];
      float right = in;

      // Compute
      const float factor = 0.3333f;
      const float res = factor * (left + center + right);

      // Update registers
      prev[0] = prev[1];
      prev[1] = right;

      // Write downstream
      write_channel_intel(systolicChannels[d + 1], res);
    }
  }
}
