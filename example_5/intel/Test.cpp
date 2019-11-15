#include <hlslib/intel/OpenCL.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "Example5.h"

void Reference(TYPE_T const a[], TYPE_T const b[], TYPE_T c[]) {
  for (int n = 0; n < DIM_N; ++n) {
    for (int m = 0; m < DIM_M; ++m) {
      c[n * DIM_M + m] = 0;
      for (int k = 0; k < DIM_K; ++k) {
        c[n * DIM_M + m] += a[n * DIM_K + k] * b[k * DIM_M + m];
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <fpga_binary_path>" << std::endl;
    return -1;
  }

  std::vector<TYPE_T, hlslib::ocl::AlignedAllocator<TYPE_T, 4096>> A(DIM_N *
                                                                     DIM_K);
  std::vector<TYPE_T, hlslib::ocl::AlignedAllocator<TYPE_T, 4096>> B(DIM_K *
                                                                     DIM_M);
  std::vector<TYPE_T, hlslib::ocl::AlignedAllocator<TYPE_T, 4096>> C_fpga(
      DIM_N * DIM_M);

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<TYPE_T> dist;
  std::for_each(A.begin(), A.end(), [&](TYPE_T &i) { i = dist(rng); });
  std::for_each(B.begin(), B.end(), [&](TYPE_T &i) { i = dist(rng); });

  // init OpenCL environment
  hlslib::ocl::Context context;

  auto program = context.MakeProgram(argv[1]);

  auto A_buff = context.MakeBuffer<TYPE_T, hlslib::ocl::Access::read>(
      A.cbegin(), A.cend());
  auto B_buff = context.MakeBuffer<TYPE_T, hlslib::ocl::Access::read>(
      B.cbegin(), B.cend());
  auto C_buff =
      context.MakeBuffer<TYPE_T, hlslib::ocl::Access::write>(DIM_N * DIM_M);

  auto kernel =
      program.MakeKernel("MatrixMultiplication", A_buff, B_buff, C_buff);

  // set kernel args and run
  kernel.ExecuteTask();

  // get data back
  C_buff.CopyToHost(C_fpga.begin());

  // correctness check
  std::vector<TYPE_T> C_ref(DIM_N * DIM_M, 0);

  // Reference implementation for comparing the result
  Reference(A.data(), B.data(), C_ref.data());

  // Verify correctness
  for (int i = 0; i < DIM_N * DIM_M; ++i) {
    const auto diff = std::abs(C_ref[i] - C_fpga[i]);
    if (diff >= 1e-3) {
      std::cout << "Mismatch at (" << i / DIM_M << ", " << i % DIM_M
                << "): " << C_fpga[i] << " (should be " << C_ref[i] << ").\n";
      return 1;
    }
  }
  std::cout << "Test ran successfully.\n";

  return 0;
}
