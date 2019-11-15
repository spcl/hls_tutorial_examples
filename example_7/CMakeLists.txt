xilinx_synthesis_target("Example7" "" "MatrixMultiplication" "Entry.cpp")
xilinx_testbench("Test.cpp Example7.cpp" "Example7")
