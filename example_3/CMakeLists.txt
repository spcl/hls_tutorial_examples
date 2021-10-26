xilinx_synthesis_target("Example3" "Stencil2D" "Entry.cpp")
xilinx_synthesis_target("Example3_Time" "Stencil2D" "Entry.cpp")
xilinx_testbench("Test.cpp Entry.cpp" "Example3")
