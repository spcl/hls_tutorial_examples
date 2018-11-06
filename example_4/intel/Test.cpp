  #include <algorithm>
  #include <iostream>
  #include <random>
  #include <vector>
  #include <ocl_utils.hpp>
  #include "Example4.h"


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
      out[N - D + i] = 0;
    }
  }

  int main(int argc, char *argv[]) {

    if(argc < 2)
    {
      std::cerr << "Usage: " <<argv[0] << " <fpga_binary_path>" <<std::endl;
      return -1;
    }


    float *in,*out_res;
    posix_memalign((void **)&in, IntelFPGAOCLUtils::AOCL_ALIGNMENT, N*sizeof(float));
    posix_memalign((void **)&out_res, IntelFPGAOCLUtils::AOCL_ALIGNMENT, N*sizeof(float));

    //init data
    std::random_device rd;
    std::default_random_engine rng(time(NULL));
    std::uniform_real_distribution<float> dist;
    std::for_each(in, in+N, [&](float &i) { i = dist(rng); });

    //init OpenCL environment
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::Program program;
    std::vector<std::string> kernel_names={"ReadMemory","WriteMemory"};
    std::vector<cl::Kernel> kernels;
    std::vector<cl::CommandQueue> queues;
    IntelFPGAOCLUtils::initEnvironment(platform,device,context,program,std::string(argv[1]),kernel_names,kernels,queues);
    

    //Allocate and copy data to FPGA
    cl::Buffer in_buff(context, CL_MEM_READ_ONLY,N*sizeof(float));
    cl::Buffer out_buff(context, CL_MEM_WRITE_ONLY,N*sizeof(float));
    queues[0].enqueueWriteBuffer(in_buff,CL_TRUE,0,N*sizeof(float),in);

    //set kernel args and run
    kernels[0].setArg(0,sizeof(cl_mem),&in_buff);
    kernels[1].setArg(0,sizeof(cl_mem),&out_buff);
    queues[0].enqueueTask(kernels[0]);
    queues[1].enqueueTask(kernels[1]);

    queues[0].finish();
    queues[1].finish();

    //get data back
    queues[0].enqueueReadBuffer(out_buff,CL_TRUE,0,N*sizeof(float),out_res);

    //check
    float *out_ref=new float[N]();
    Reference(in, out_ref);

    for (int i = 0; i < N; ++i) {
      const auto diff = std::abs(out_ref[i] - out_res[i]);
      if (diff > 1e-4) {
        std::cerr << "Mismatch at " << i << ": " << out_res[i]
                  << " (should be " << out_ref[i] << ")." << std::endl;
        return 1;
      }
    }
    std::cout << "Test ran successfully.\n";

    delete out_ref;
    free(in);
    free(out_res);
    return 0;
  }
