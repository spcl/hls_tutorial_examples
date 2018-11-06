CXX		= g++
AOCXX	= aoc
CXXFLAGS= -std=c++11 -g
BOARD 	= p520_max_sg280l
AOCXXFLAGS  = -march=emulator -v -board=$(BOARD) 
# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )

.PHONY: all clean run_test

all: report

report: Example4.cl 
	$(AOCXX) -v -board=$(BOARD) -rtl $< -report 

emulated: Example4.cl
	$(AOCXX) $(AOCXXFLAGS) -o emulated_Example4.aocx Example4.cl 

test: Test.cpp Example4.cl
	c++ Test.cpp -std=c++11 -I../../intel_ocl_utils $(AOCL_COMPILE_CONFIG) $(AOCL_LINK_CONFIG) -o Example4

run_test: test emulated
	env CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 ./Example4 emulated_Example4.aocx

clean:
	rm -rf Example4/ || true
	rm -rf emulated_Example4/ || true
	rm -f *Example4*.ao* || true
	rm -rf Example4*.temp || true



