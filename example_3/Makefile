all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/example3/syn/report/Stencil2D_csynth.rpt report.rpt

synthesis_time:
	vivado_hls -f SynthesisTime.tcl
	cp hls_time/example3/syn/report/Stencil2D_csynth.rpt report_time.rpt

test: example3.cpp entry.cpp test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ test.cpp example3.cpp entry.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o example3
	./example3

clean:
	rm -r hls || true
	rm -r hls_time || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm report_time.rpt || true
	rm example3

.PHONY: all clean
