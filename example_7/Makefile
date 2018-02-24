all:
	vivado_hls -f Synthesis.tcl

test:
	clang++ example7.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -o example7.out
	./example7.out

clean:
	rm -r example7 || true
	rm example7.out || true
