#define N 1024

__kernel void Simple1DStencil(__global float const *restrict memory_in,
                              __global float *restrict memory_out) {

  // The Intel compiler is able to achieve II=1 for this loop
  // The three loads are coalesced into a single one

  for (int i = 1; i < N - 1; ++i) {

    float left = memory_in[i - 1];
    float center = memory_in[i];
    float right = memory_in[i + 1];

    const float factor = 0.3333f;
    float average = factor * (left + center + right);

    memory_out[i] = average;
  }
}
