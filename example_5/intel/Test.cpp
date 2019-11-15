#include "Example5.h"
#include <algorithm>
#include <iostream>
#include <ocl_utils.hpp>
#include <random>
#include <vector>

void Reference(TYPE_T const a[], TYPE_T const b[], TYPE_T c[]) {

  for (int n = 0; n < N; ++n) {
    for (int m = 0; m < M; ++m) {
      c[n * M + m] = 0;
      for (int k = 0; k < K; ++k) {
        c[n * M + m] += a[n * K + k] * b[k * M + m];
      }
    }
  }
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <fpga_binary_path>" << std::endl;
    return -1;
  }

  TYPE_T *A, *B, *C_fpga;
  posix_memalign((void **)&A, IntelFPGAOCLUtils::AOCL_ALIGNMENT,
                 N * K * sizeof(TYPE_T));
  posix_memalign((void **)&B, IntelFPGAOCLUtils::AOCL_ALIGNMENT,
                 K * M * sizeof(TYPE_T));
  posix_memalign((void **)&C_fpga, IntelFPGAOCLUtils::AOCL_ALIGNMENT,
                 N * M * sizeof(TYPE_T));

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<TYPE_T> dist;
  std::for_each(A, A + N * K, [&](TYPE_T &i) { i = dist(rng); });
  std::for_each(B, B + K * M, [&](TYPE_T &i) { i = dist(rng); });

  // init OpenCL environment
  cl::Platform platform;
  cl::Device device;
  cl::Context context;
  cl::Program program;
  std::vector<std::string> kernel_names = {"MatrixMultiplication"};
  std::vector<cl::Kernel> kernels;
  std::vector<cl::CommandQueue> queues;
  IntelFPGAOCLUtils::initEnvironment(platform, device, context, program,
                                     std::string(argv[1]), kernel_names,
                                     kernels, queues);

  // Allocate and copy data to FPGA
  cl::Buffer A_buff(context, CL_MEM_READ_ONLY, N * K * sizeof(TYPE_T));
  cl::Buffer B_buff(context, CL_MEM_READ_ONLY, K * M * sizeof(TYPE_T));
  cl::Buffer C_buff(context, CL_MEM_WRITE_ONLY, N * M * sizeof(TYPE_T));

  queues[0].enqueueWriteBuffer(A_buff, CL_TRUE, 0, N * K * sizeof(TYPE_T), A);
  queues[0].enqueueWriteBuffer(B_buff, CL_TRUE, 0, K * M * sizeof(TYPE_T), B);

  // set kernel args and run
  kernels[0].setArg(0, sizeof(cl_mem), &A_buff);
  kernels[0].setArg(1, sizeof(cl_mem), &B_buff);
  kernels[0].setArg(2, sizeof(cl_mem), &C_buff);

  queues[0].enqueueTask(kernels[0]);

  queues[0].finish();

  // get data back
  queues[0].enqueueReadBuffer(C_buff, CL_TRUE, 0, N * M * sizeof(TYPE_T),
                              C_fpga);

  // correctness check
  TYPE_T *C_ref = new TYPE_T[N * M]{0};

  // Reference implementation for comparing the result
  Reference(A, B, C_ref);

  // Verify correctness
  for (int i = 0; i < N * M; ++i) {
    const auto diff = std::abs(C_ref[i] - C_fpga[i]);
    if (diff >= 1e-3) {
      std::cout << "Mismatch at (" << i / M << ", " << i % M
                << "): " << C_fpga[i] << " (should be " << C_ref[i] << ").\n";
      return 1;
    }
  }
  std::cout << "Test ran successfully.\n";

  return 0;
}
