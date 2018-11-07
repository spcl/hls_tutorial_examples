## Xilinx

### Dependencies

After cloning this repository, make sure you clone the [hlslib](https://github.com/definelicht/hlslib) submodule dependency, by executing the following command:

```
git submodule update --init
```

To run high-level synthesis, the `vivado_hls` tool must be available on the commandline. This can be downloaded from the Xilinx website, either as part of the [Vivado Design Suite](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/) or of [SDAccel](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/sdx-development-environments.html).

### Compiling the code

Each example is equipped with a simple makefile, which typically includes the targets `synthesis` and `test`. To run high-level synthesis on the code sample, simply run `make` in the appropriate folder.

## Intel

Coming soon...
