open_project hls 
open_solution solution 
set_part xcku5p-sfvb784-3-e 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "example6.cpp" 
set_top Entry 
create_clock -period 250MHz -name default
csynth_design
exit
