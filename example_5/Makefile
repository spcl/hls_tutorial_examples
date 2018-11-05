all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/Example5/syn/report/MatrixMultiplication_csynth.rpt report.rpt

synthesis_reordered:
	vivado_hls -f SynthesisReordered.tcl
	cp hls_reordered/Example5/syn/report/MatrixMultiplication_csynth.rpt report_reordered.rpt

test: Example5.cpp Entry.cpp Test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ Test.cpp Example5.cpp Entry.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o Example5
	./Example5

clean:
	rm -r hls || true
	rm -r hls_reordered || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm report_reordered.rpt || true
	rm Example5 || true

.PHONY: all clean
