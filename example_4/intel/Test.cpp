#include <hlslib/intel/OpenCL.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "Example4.h"

void Reference(float const in[], float out[]) {
  float tmp[DIM_N];
  std::copy(in, in + DIM_N, tmp);
  for (int d = 0; d < DEPTH; ++d) {
    auto read = (d % 2 == 0) ? tmp : out;
    auto write = (d % 2 == 0) ? out : tmp;
    for (int i = d; i < DIM_N - d; ++i) {
      write[i] = 0.3333 * (read[i - 1] + read[i] + read[i + 1]);
    }
  }
  if (DEPTH % 2 == 0) {
    std::copy(tmp, tmp + DIM_N, out);
  }
  for (int i = 0; i < DEPTH; ++i) {
    out[i] = 0;
    out[DIM_N - DEPTH + i] = 0;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <fpga_binary_path>" << std::endl;
    return -1;
  }

  std::vector<float, hlslib::ocl::AlignedAllocator<float, 4096>> in(DIM_N);
  std::vector<float, hlslib::ocl::AlignedAllocator<float, 4096>> out_res(DIM_N);

  // init data
  std::random_device rd;
  std::default_random_engine rng(time(NULL));
  std::uniform_real_distribution<float> dist;
  std::for_each(in.begin(), in.end(), [&](float &i) { i = dist(rng); });

  // init OpenCL environment
  hlslib::ocl::Context context;

  auto program = context.MakeProgram(argv[1]);

  // Allocate and copy data to FPGA
  auto in_buff = context.MakeBuffer<float, hlslib::ocl::Access::read>(
      in.cbegin(), in.cend());
  auto out_buff = context.MakeBuffer<float, hlslib::ocl::Access::write>(DIM_N);

  auto kernel_read = program.MakeKernel("ReadMemory", in_buff);
  auto kernel_write = program.MakeKernel("WriteMemory", out_buff);

  // Execute kernel
  auto future_read = kernel_read.ExecuteTaskAsync();
  auto future_write = kernel_write.ExecuteTaskAsync();
  future_read.get();
  future_write.get();

  // get data back
  out_buff.CopyToHost(out_res.begin());

  // check
  std::vector<float> out_ref(DIM_N);
  Reference(in.data(), out_ref.data());

  for (int i = 0; i < DIM_N; ++i) {
    const auto diff = std::abs(out_ref[i] - out_res[i]);
    if (diff > 1e-4) {
      std::cerr << "Mismatch at " << i << ": " << out_res[i] << " (should be "
                << out_ref[i] << ")." << std::endl;
      return 1;
    }
  }
  std::cout << "Test ran successfully.\n";

  return 0;
}
