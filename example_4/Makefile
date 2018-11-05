all: synthesis

synthesis:
	vivado_hls -f Synthesis.tcl
	cp hls/example4/syn/report/Entry_csynth.rpt report.rpt

test: example4.cpp test.cpp
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	c++ test.cpp example4.cpp -std=c++11 -I../hlslib/include -I$(VIVADO_HLS_DIR)/include -lpthread -o example4
	./example4

clean:
	rm -r hls || true
	rm vivado_hls.log || true
	rm report.rpt || true
	rm example4

.PHONY: all clean
