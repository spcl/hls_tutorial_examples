### Xilinx

After cloning this repository, make sure you clone the [hlslib](https://github.com/definelicht/hlslib) submodule dependency, by executing the following command:

```
git submodule update --init
```

To run high-level synthesis, the `vivado_hls` tool must be available on the commandline. This can be downloaded from the Xilinx website, either as part of the [Vivado Design Suite](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/) or of [SDAccel](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/sdx-development-environments.html). Download the tools requires registering a (free) account with Xilinx.

Each example is equipped with a simple makefile, which typically includes the targets `synthesis` and `test`. To run high-level synthesis on the code sample, simply run `make` in the appropriate folder.

### Intel FPGA

To run high-level synthesis and generate the performance and utilization report for the Intel FPGA examples, simple makefiles are provided, assuming that the [Intel FPGA OpenCL SDK](https://www.intel.com/content/www/us/en/software/programmable/sdk-for-opencl/overview.html) is installed and on the path. Download these tools requires registering a (free) account with Intel.

Makefiles typically includes the targets `report` and `run_test`.

