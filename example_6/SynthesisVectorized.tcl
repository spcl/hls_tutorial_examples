open_project hls_vectorized 
open_solution example6 
set_part xcvu9p-flgb2104-2-i 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "Example6_Vectorized.cpp Entry.cpp" 
set_top EntryVectorized 
create_clock -period 300MHz -name default
csynth_design
exit
