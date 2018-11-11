open_project hls 
open_solution example7 
set_part xcku5p-sfvb784-3-e 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "Example7.cpp" 
set_top Entry  
create_clock -period 300MHz -name default
csynth_design
exit
