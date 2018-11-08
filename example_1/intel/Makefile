CXX     = g++
AOCXX   = aoc
CXXFLAGS= -std=c++11 -g
BOARD 	= s5_ref
AOCXXFLAGS  = -march=emulator -board=$(BOARD) 
# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )

.PHONY: all clean

all: report

report: Example1.cl 
	$(AOCXX) -board=$(BOARD) -c $< -report
	rm -rf reports || true
	cp -r Example1/reports .

clean:
	rm -rf Example1/ || true
	rm -f *Example1*.ao* || true
	rm -rf Example1*.temp || true
	rm -rf report* || true



