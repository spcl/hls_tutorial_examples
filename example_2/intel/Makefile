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

clean:
	rm -rf Example2/ || true
	rm -rf EmulatedExample2/ || true
	rm -f *Example2*.ao* || true
	rm -rf Example2*.temp || true
	rm -rf report* || true



