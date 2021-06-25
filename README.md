### Content

These are examples used in the tutorial _Productive Parallel Programming on FPGA
with High-Level Synthesis_, given at PPoPP'18, SC'18, SC'19, HiPEAC'20, SC'20,
ISC'21, and SC'21.

For comprehensive coverage of HLS transformations for HPC, we refer to our work
_Transformations of High-Level Synthesis Codes for High-Performance Computing_,
[available on arXiv](https://arxiv.org/abs/1805.08288) [1]. 

### Dependencies 

These examples depend on [hlslib](https://github.com/definelicht/hlslib) as a
submodule [2]. Make sure you clone with the `--recursive` flag, or run `git
submodule update --init` after cloning.

All examples use CMake to configure and build both Xilinx and Intel kernels. The
configuration relies on scripts to find the Xilinx and/or Intel tools on your
system by looking for the `xocc` and `aoc` binaries, respectively. To ensure
that the tools are found, make sure these are on your `PATH`.

### Building and running

To build the examples, create a build folder and configure it using CMake:

```bash
mkdir build
cd build
cmake ..
```

Depending on your installation of the Intel FPGA tools, you might have to
specify a board package available to your installation, e.g.,
`-DINTEL_FPGA_BOARD=p520_hpc_sg280l`, to build the Intel emulation kernels.

Each example has one or more synthesis targets (we recommend using a shell that
can autocomplete GNU make targets). For example, from your build directory:

```bash
cd example_0
make synthesize_example0
```

Each Xilinx synthesis target will also copy a `report.rpt`-file into the build
directory, which you can inspect to get more detailed information about the
resulting architecture.

Intel synthesis targets are suffixed with `_intel`, and are not available for all
examples. Reports are generated in a subdirectory named after the kernel, e.g., `Example2/reports`. 

### References

- [1] Johannes de Fine Licht, Maciej Besta, Simon Meierhans, and Torsten Hoefler. _"Transformations of High-Level Synthesis Codes for High-Performance Computing."_ IEEE Transactions on Parallel and Distributed Systems (TPDS), Vol. 32, Issue 5, 2021 ([arXiv link](https://arxiv.org/abs/1805.08288)).
- [2] Johannes de Fine Licht and Torsten Hoefler. _"hlslib: Software Engineering for Hardware Design"_. arXiv preprint [arXiv:1910.04436](https://arxiv.org/abs/1910.04436) (2019).
