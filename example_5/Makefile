all:
	vivado_hls -f Synthesis.tcl

reordered:
	vivado_hls -f SynthesisReordered.tcl

test:
	clang++ example5.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -o example5.out
	./example5.out

test_reordered:
	clang++ example5.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -DUSE_REORDERED -o example5.out
	./example5.out

clean:
	rm -r example5 || true
	rm example5.out || true
