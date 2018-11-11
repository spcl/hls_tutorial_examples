open_project hls_pipelined 
open_solution example1 
set_part xcku5p-sfvb784-3-e 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "Example1_Pipelined.cpp Entry.cpp" 
set_top Entry 
create_clock -period 300MHz -name default
csynth_design
exit
