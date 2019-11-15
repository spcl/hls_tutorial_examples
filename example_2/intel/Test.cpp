
#include <algorithm>
#include <iostream>
#include <ocl_utils.hpp>
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

  float *in, *out_res;
  posix_memalign((void **)&in, IntelFPGAOCLUtils::AOCL_ALIGNMENT,
                 DIM_N * DIM_M * sizeof(float));
  posix_memalign((void **)&out_res, IntelFPGAOCLUtils::AOCL_ALIGNMENT,
                 DIM_N * DIM_M * sizeof(float));

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(in, in + DIM_N * DIM_M, [&](float &i) { i = dist(rng); });

  float *out_ref = new float[DIM_N * DIM_M]();

  // init OpenCL environment
  cl::Platform platform;
  cl::Device device;
  cl::Context context;
  cl::Program program;
  std::vector<std::string> kernel_names = {"Stencil2D"};
  std::vector<cl::Kernel> kernels;
  std::vector<cl::CommandQueue> queues;
  IntelFPGAOCLUtils::initEnvironment(platform, device, context, program,
                                     std::string(argv[1]), kernel_names,
                                     kernels, queues);

  // Allocate and copy data to FPGA
  cl::Buffer in_buff(context, CL_MEM_READ_ONLY, DIM_N * DIM_M * sizeof(float));
  cl::Buffer out_buff(context, CL_MEM_WRITE_ONLY,
                      DIM_N * DIM_M * sizeof(float));
  queues[0].enqueueWriteBuffer(in_buff, CL_TRUE, 0,
                               DIM_N * DIM_M * sizeof(float), in);

  // set kernel args and run
  kernels[0].setArg(0, sizeof(cl_mem), &in_buff);
  kernels[0].setArg(1, sizeof(cl_mem), &out_buff);
  queues[0].enqueueTask(kernels[0]);

  queues[0].finish();

  // get data back
  queues[0].enqueueReadBuffer(out_buff, CL_TRUE, 0,
                              DIM_N * DIM_M * sizeof(float), out_res);

  // correctness check
  Reference(in, out_ref);

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
