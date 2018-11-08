CXX	    = g++
AOCXX   = aoc
CXXFLAGS= -std=c++11 -g
BOARD 	= s5_ref
AOCXXFLAGS  = -march=emulator -board=$(BOARD) 
# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )

.PHONY: all clean

all: report 

report: Example4.cl 
	$(AOCXX) -v -board=$(BOARD) -c $< -report
	cp -r Example4/reports .

emulated: Example4.cl
	$(AOCXX) $(AOCXXFLAGS) -o EmulatedExample4.aocx Example4.cl 

test: Test.cpp Example4.cl
	$(CXX) Test.cpp $(CXXFLAGS) -I../../intel_ocl_utils $(AOCL_COMPILE_CONFIG) $(AOCL_LINK_CONFIG) -o TestExample4

run_test: test emulated
	LD_LIBRARY_PATH=$(INTELFPGAOCLSDKROOT)/board/s5_ref/linux64/lib:$(INTELFPGAOCLSDKROOT)/host/linux64/lib:$(LD_LIBRARY_PATH) \
	env CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 ./TestExample4 EmulatedExample4.aocx

clean:
	rm -rf Example4/ || true
	rm -rf EmulatedExample4/ || true
	rm -f *Example4*.ao* || true
	rm -rf Example4*.temp || true
	rm TestExample4 || true
	rm -rf report* || true



