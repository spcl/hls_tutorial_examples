xilinx_synthesis_target("Example4" "" "Example4" "")
xilinx_testbench("Test.cpp" "Example4")
intel_synthesis_target("Example4")
intel_testbench("Test.cpp" "Example4")
