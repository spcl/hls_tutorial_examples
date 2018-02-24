all:
	vivado_hls -f Synthesis.tcl

test:
	clang++ example4.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -o example4.out
	./example4.out

clean:
	rm -r example4 || true
	rm example4.out || true
