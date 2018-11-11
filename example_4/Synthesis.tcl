open_project hls 
open_solution example4 
set_part xcku5p-sfvb784-3-e
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "Example4.cpp" 
set_top Entry 
create_clock -period 300MHz -name default
config_rtl -disable_start_propagation
csynth_design
exit
