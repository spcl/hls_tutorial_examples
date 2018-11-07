CXX		= g++
AOCXX	= aoc
CXXFLAGS= -std=c++11 -g
BOARD 	= a10gx
AOCXXFLAGS  = -march=emulator -board=$(BOARD) 
# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )

.PHONY: all clean

all: report

report: Example2.cl 
	$(AOCXX) -v -board=$(BOARD) -rtl $< -report
	cp -r Example2/reports .

report_buffered: Example2_Buffered.cl
	$(AOCXX) -v -board=$(BOARD) -rtl $< -report
	cp -r Example2_Buffered/reports ./reports_buffered


emulated: Example2.cl
	$(AOCXX) $(AOCXXFLAGS) -o EmulatedExample2.aocx Example2.cl 

test: Test.cpp Example2.cl
	$(CXX) Test.cpp $(CXXFLAGS) -I../../intel_ocl_utils $(AOCL_COMPILE_CONFIG) $(AOCL_LINK_CONFIG) -o TestExample2

run_test: test emulated
	env CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 ./TestExample2 EmulatedExample2.aocx

clean:
	rm -rf Example2*/ || true
	rm -rf EmulatedExample2/ || true
	rm -f *Example2*.ao* || true
	rm -rf Example2*.temp || true
	rm -rf report* || true
	rm -rf TestExample2 || true



