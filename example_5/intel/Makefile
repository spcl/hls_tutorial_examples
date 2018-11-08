CXX	    = g++
AOCXX   = aoc
CXXFLAGS= -std=c++11 -g
BOARD 	= s5_ref
AOCXXFLAGS  = -march=emulator -board=$(BOARD) 
# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )

.PHONY: all clean

all: report report_reordered

report: Example5.cl Example5.h
	$(AOCXX) -v -board=$(BOARD) -c $< -report
	rm -rf reports || true
	cp -r Example5/reports ./

report_reordered: Example5_Reordered.cl Example5.h
	$(AOCXX) -v -board=$(BOARD) -c $< -report
	rm -fr report_reordered || true
	cp -r Example5_Reordered/reports ./report_reordered

emulated: Example5.cl Example5.h
	$(AOCXX) $(AOCXXFLAGS) -o EmulatedExample5.aocx Example5.cl 

test: Test.cpp Example5.cl
	$(CXX) Test.cpp $(CXXFLAGS) -I../../intel_ocl_utils $(AOCL_COMPILE_CONFIG) $(AOCL_LINK_CONFIG) -o TestExample5

run_test: test emulated
	LD_LIBRARY_PATH=$(INTELFPGAOCLSDKROOT)/board/s5_ref/linux64/lib:$(INTELFPGAOCLSDKROOT)/host/linux64/lib:$(LD_LIBRARY_PATH) \
	env CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 ./TestExample5 EmulatedExample5.aocx

clean:
	rm -rf Example5/ || true
	rm -rf EmulatedExample5/ || true
	rm -f *Example5*.ao* || true
	rm -rf Example5*.temp || true
	rm TestExample5 || true
	rm -rf report* || true
	rm -rf Example5_Reordered/ ||true



