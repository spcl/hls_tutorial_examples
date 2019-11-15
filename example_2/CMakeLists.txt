xilinx_synthesis_target("Example2" "" "Stencil2D" "Entry.cpp")
xilinx_synthesis_target("Example2_Buffered" "" "Stencil2D" "Entry.cpp")
xilinx_testbench("Test.cpp Entry.cpp" "Example2")
