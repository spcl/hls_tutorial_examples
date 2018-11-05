all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/Example3/syn/report/Stencil2D_csynth.rpt report.rpt

synthesis_time:
	vivado_hls -f SynthesisTime.tcl
	cp hls_time/Example3/syn/report/Stencil2D_csynth.rpt report_time.rpt

test: Example3.cpp Entry.cpp Test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ Test.cpp Example3.cpp Entry.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o Example3
	./Example3

clean:
	rm -r hls || true
	rm -r hls_time || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm report_time.rpt || true
	rm Example3

.PHONY: all clean
