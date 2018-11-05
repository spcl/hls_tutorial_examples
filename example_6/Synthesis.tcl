open_project hls 
open_solution example6 
set_part xcvu9p-flgb2104-2-i 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "Example6.cpp Entry.cpp" 
set_top Entry 
create_clock -period 300Hz -name default
csynth_design
exit
