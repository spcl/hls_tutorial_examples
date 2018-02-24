all:
	vivado_hls -f Synthesis.tcl

buffered:
	vivado_hls -f SynthesisBuffered.tcl

streams:
	vivado_hls -f SynthesisStreams.tcl

test:
	clang++ example2.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -o example2.out
	./example2.out

clean:
	rm -r example2 || true
	rm example2.out || true
	rm vivado_hls.log || true
