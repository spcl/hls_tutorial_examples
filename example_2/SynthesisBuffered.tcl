open_project example2
open_solution example2_buffered
set_part xcku115-flvb2104-2-e 
add_files -cflags "-DHLSLIB_SYNTHESIS -DUSE_BUFFERED -std=c++11 -I../hlslib/include" "example2.cpp" 
set_top Entry 
create_clock -period 250MHz -name default
csynth_design
exit
