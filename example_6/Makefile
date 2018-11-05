all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/example6/syn/report/MatrixMultiplication_csynth.rpt report.rpt

synthesis_vectorized:
	vivado_hls -f SynthesisVectorized.tcl
	cp hls_vectorized/example6/syn/report/MatrixMultiplication_csynth.rpt report_vectorized.rpt

test: Example6.cpp Entry.cpp Test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ Test.cpp Example6_Vectorized.cpp Example6.cpp Entry.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o Example6
	./Example6

clean:
	rm -r hls || true
	rm -r hls_vectorized || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm report_vectorized.rpt || true
	rm Example6 || true

.PHONY: all clean
