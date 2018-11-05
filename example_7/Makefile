all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/example7/syn/report/Entry_csynth.rpt report.rpt

test: Example7.cpp Test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ Test.cpp Example7.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o Example7
	./Example7

clean:
	rm -r hls || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm Example7 || true

.PHONY: all clean
