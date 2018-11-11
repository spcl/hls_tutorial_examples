open_project hls_vectorized 
open_solution example6 
set_part xcku5p-sfvb784-3-e 
add_files -cflags "-DHLSLIB_SYNTHESIS -std=c++11 -I../hlslib/include" "Example6_Vectorized.cpp Entry.cpp" 
set_top EntryVectorized 
create_clock -period 300MHz -name default
config_rtl -disable_start_propagation
csynth_design
exit
