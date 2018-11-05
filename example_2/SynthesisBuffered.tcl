open_project hls_buffered 
open_solution example2 
set_part xcvu9p-flgb2104-2-i 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "example2_buffered.cpp entry.cpp" 
set_top Entry 
create_clock -period 300MHz -name default
csynth_design
exit
