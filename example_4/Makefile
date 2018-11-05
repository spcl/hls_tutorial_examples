all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/example4/syn/report/Entry_csynth.rpt report.rpt

test: Example4.cpp Test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ Test.cpp Example4.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o Example4
	./Example4

clean:
	rm -r hls || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm Example4

.PHONY: all clean
