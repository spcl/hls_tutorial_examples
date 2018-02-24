all:
	vivado_hls -f Synthesis.tcl

time:
	vivado_hls -f SynthesisTime.tcl

test:
	clang++ example3.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -o example3.out
	./example3.out

clean:
	rm -r example3 || true
	rm example3.out || true
	rm vivado_hls.log || true
