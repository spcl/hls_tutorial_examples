all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/Example2/syn/report/Stencil2D_csynth.rpt report.rpt

synthesis_buffered:
	vivado_hls -f SynthesisBuffered.tcl
	cp hls_buffered/Example2/syn/report/Stencil2D_csynth.rpt report_buffered.rpt

synthesis_streams:
	vivado_hls -f SynthesisStreams.tcl
	cp hls_streams/Example2/syn/report/Stencil2D_csynth.rpt report_streams.rpt

test: Example2.cpp Entry.cpp Test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ Test.cpp Example2.cpp Entry.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o Example2
	./Example2

clean:
	rm -r hls || true
	rm -r hls_buffered || true
	rm -r hls_streams || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm report_buffered.rpt || true
	rm report_streams.rpt || true
	rm Example2 || true

.PHONY: all clean
