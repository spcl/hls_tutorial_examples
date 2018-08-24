open_project hls 
open_solution solution 
set_part xcvu9p-flgb2104-2-i 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "example0.cpp" 
set_top Entry 
create_clock -period 250MHz -name default
csynth_design
exit
