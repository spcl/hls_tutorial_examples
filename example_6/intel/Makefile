CXX		= g++
AOCXX	= aoc
CXXFLAGS= -std=c++11 -g
BOARD 	= s5_ref
AOCXXFLAGS  = -march=emulator -board=$(BOARD) 
# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )

.PHONY: all clean

all: report

report: Example6.cl Example6.h
	$(AOCXX) -v -board=$(BOARD) -c $< -report
	cp -r Example6/reports ./

clean:
	rm -rf Example6/ || true
	rm -rf EmulatedExample6/ || true
	rm -f *Example6*.ao* || true
	rm -rf Example6*.temp || true
	rm TestExample6 || true
	rm -rf report* || true
	rm -rf Example6_Reordered/ ||true



