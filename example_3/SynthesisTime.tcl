open_project example3
open_solution example3_time
set_part xcku5p-sfvb784-3-e 
add_files -cflags "-DHLSLIB_SYNTHESIS -DUSE_TIME -std=c++11 -I../hlslib/include" "example3.cpp" 
set_top Entry 
create_clock -period 250MHz -name default
csynth_design
exit
