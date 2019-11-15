xilinx_synthesis_target("Example5" "" "MatrixMultiplication" "Entry.cpp")
xilinx_synthesis_target("Example5_Reordered" "" "MatrixMultiplication" "Entry.cpp")
xilinx_testbench("Test.cpp Entry.cpp" "Example5")
