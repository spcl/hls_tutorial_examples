xilinx_synthesis_target("Example6" "" "MatrixMultiplication" "Entry.cpp")
xilinx_synthesis_target("Example6_Vectorized" "" "MatrixMultiplication" "Entry.cpp")
xilinx_testbench("Test.cpp Entry.cpp Example6.cpp Example6_Vectorized.cpp" "Example6")
