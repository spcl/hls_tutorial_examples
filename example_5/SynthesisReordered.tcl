open_project hls_reordered
open_solution example5 
set_part xcvu9p-flgb2104-2-i
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "Example5_Reordered.cpp Entry.cpp" 
set_top Entry 
create_clock -period 300MHz -name default
csynth_design
exit
