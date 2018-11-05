open_project hls_time 
open_solution example3 
set_part xcvu9p-flgb2104-2-i
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "example3_time.cpp entry.cpp" 
set_top Entry 
create_clock -period 300MHz -name default
csynth_design
exit
