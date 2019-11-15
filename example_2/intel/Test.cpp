#include <hlslib/intel/OpenCL.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "Example2.h"

void Reference(float const in[], float out[]) {
  for (int i = 1; i < DIM_N - 1; ++i) {
    for (int j = 0; j < DIM_M; ++j) {
      out[i * DIM_M + j] =
          0.3333 * (in[(i - 1) * DIM_M + j] + in[i * DIM_M + j] +
                    in[(i + 1) * DIM_M + j]);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <fpga_binary_path>" << std::endl;
    return -1;
  }

  std::vector<float, hlslib::ocl::AlignedAllocator<float, 4096>> in(DIM_N *
                                                                    DIM_M);
  std::vector<float, hlslib::ocl::AlignedAllocator<float, 4096>> out_res(DIM_N *
                                                                         DIM_M);

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(in.begin(), in.end(), [&](float &i) { i = dist(rng); });

  std::vector<float> out_ref(DIM_N * DIM_M);

  // init OpenCL environment
  hlslib::ocl::Context context;

  auto program = context.MakeProgram(argv[1]);

  // Allocate and copy data to FPGA
  auto in_buff = context.MakeBuffer<float, hlslib::ocl::Access::read>(
      in.cbegin(), in.cend());
  auto out_buff =
      context.MakeBuffer<float, hlslib::ocl::Access::write>(DIM_N * DIM_M);

  auto kernel = program.MakeKernel("Stencil2D", in_buff, out_buff);

  // Run kernel
  kernel.ExecuteTask();

  // Copy data back
  out_buff.CopyToHost(out_res.begin());

  // correctness check
  Reference(in.data(), out_ref.data());

  for (int i = 0; i < DIM_N; ++i) {
    for (int j = 0; j < DIM_M; ++j) {
      const auto diff =
          std::abs(out_ref[i * DIM_M + j] - out_res[i * DIM_M + j]);
      if (diff > 1e-4) {
        std::cerr << "Mismatch at (" << i << ", " << j
                  << "): " << out_res[i * DIM_M + j] << " (should be "
                  << out_ref[i * DIM_M + j] << ")." << std::endl;
        return 1;
      }
    }
  }
  std::cout << "Test ran successfully.\n";

  return 0;
}
