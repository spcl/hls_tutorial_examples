all:
	vivado_hls -f Synthesis.tcl

vectorized:
	vivado_hls -f SynthesisVector.tcl

test:
	clang++ example6.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -o example6.out
	./example6.out

test_vectorized:
	clang++ example6.cpp -std=c++11 -I../hlslib/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -lpthread -DUSE_VECTORIZED -o example6.out
	./example6.out

clean:
	rm -r example6 || true
	rm example6.out || true
	rm vivado_hls.log || true
