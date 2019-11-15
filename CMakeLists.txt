cmake_minimum_required(VERSION 3.0)
project(hls_tutorial_examples)

# Options
set(XILINX_PART_NAME "xcvu9p-flgb2104-2-i" CACHE STRING "Part name for HLS.")
set(XILINX_DSA_NAME "xilinx_vcu1525_dynamic_5_1" CACHE STRING "DSA string for xocc.")

# Include find-scripts from hlslib
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/hlslib/cmake)

find_package(Threads REQUIRED)
find_package(SDAccel)
find_package(IntelFPGAOpenCL)

# Set up compiling Xilinx codes
set(XILINX_SYNTHESIS_FLAGS "-DHLSLIB_SYNTHESIS -DHLSLIB_XILINX -std=c++11 -I${CMAKE_SOURCE_DIR}/hlslib/include -I.")
include_directories(${CMAKE_SOURCE_DIR}/hlslib/include)

enable_testing()

# Sets up a synthesis target for the given kernel
function(xilinx_synthesis_target KERNEL_NAME ADDITIONAL_FLAGS REPORT_MODULE EXTRA_FILES)
  string(TOLOWER ${KERNEL_NAME} KERNEL_NAME_LOWER)
  set(HLSLIB_PROJECT_NAME "${KERNEL_NAME_LOWER}")
  set(HLSLIB_SRC_SYNTHESIS)
  string(REPLACE " " ";" EXTRA_FILES "${EXTRA_FILES}")
  foreach(CODE_FILE ${KERNEL_NAME}.cpp ${EXTRA_FILES})
    set(HLSLIB_SRC_SYNTHESIS "${HLSLIB_SRC_SYNTHESIS}
        ${CMAKE_CURRENT_SOURCE_DIR}/xilinx/${CODE_FILE}")
  endforeach()
  set(HLSLIB_ENTRY_FUNCTION "${KERNEL_NAME}")
  set(HLSLIB_TARGET_CLOCK "200")
  set(HLSLIB_PART_NAME "${XILINX_PART_NAME}")
  set(HLSLIB_SYNTHESIS_FLAGS ${XILINX_SYNTHESIS_FLAGS})
  configure_file(${CMAKE_SOURCE_DIR}/hlslib/xilinx_test/scripts/Synthesis.tcl.in
                 Synthesize${KERNEL_NAME}.tcl)
  add_custom_target(synthesize_${KERNEL_NAME_LOWER}
    COMMAND ${SDAccel_VIVADO_HLS} -f Synthesize${KERNEL_NAME}.tcl 
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/${HLSLIB_PROJECT_NAME}/${HLSLIB_PART_NAME}/syn/report/${REPORT_MODULE}_csynth.rpt
    ${CMAKE_CURRENT_BINARY_DIR}/report_${KERNEL_NAME_LOWER}.rpt) 
endfunction()

# Builds a testbench that can run Vivado HLS C++ kernels in software simulation
function(xilinx_testbench CPP_FILES KERNEL_NAME)
  string(REPLACE " " ";" CPP_FILES "${CPP_FILES}")
  set(CODE_FILES)
  foreach(CODE_FILE ${KERNEL_NAME}.cpp ${CPP_FILES})
    set(CODE_FILES ${CODE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/xilinx/${CODE_FILE})
  endforeach()
  add_executable(Test${KERNEL_NAME} ${CODE_FILES})
  target_link_libraries(Test${KERNEL_NAME} ${CMAKE_THREAD_LIBS_INIT} ${SDAccel_LIBRARIES})
  add_test(Test${KERNEL_NAME} Test${KERNEL_NAME})
endfunction()

# Sets up a SDAccel kernel build target for the given kernel
function(sdaccel_target KERNEL_NAME ADDITIONAL_FLAGS)
  set(XILINX_SYNTHESIS_FLAGS_FLAGS "${XILINX_SYNTHESIS_FLAGS} ${ADDITIONAL_FLAGS}")
  set(XOCC_COMMAND
    -s
    -O3
    ${MM_XOCC_FLAGS}
    -I${CMAKE_CURRENT_SOURCE_DIR}/include
    -I${CMAKE_SOURCE_DIR}/include
    -I${CMAKE_CURRENT_BINARY_DIR}
    -I${CMAKE_BINARY_DIR}
    --kernel ${KERNEL_NAME} 
    --platform ${XILINX_DSA_NAME}
    --xp prop:kernel.${KERNEL_NAME}.kernel_flags="${XILINX_SYNTHESIS_FLAGS_FLAGS}"
    --profile_kernel "data:all:all:all"
    --profile_kernel "stall:all:all"
    --profile_kernel "exec:all:all"
    --max_memory_ports all)
  if(((${SDAccel_MAJOR_VERSION} LESS 2018) AND (${SDAccel_MINOR_VERSION} LESS 3)) OR ${SDAccel_MAJOR_VERSION} LESS 2017)
    add_custom_target(build_${KERNEL_NAME}_hardware
      COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${SDAccel_XOCC} -t hw
      ${XOCC_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw.xclbin) 
    add_custom_target(build_${KERNEL_NAME}_hardware_emulation
      COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${SDAccel_XOCC} -t hw_emu -g
      ${XOCC_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw_emu.xclbin) 
  else()
    add_custom_target(compile_${KERNEL_NAME}_hardware
      COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${SDAccel_XOCC} -c -t hw
      ${XOCC_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw.xo) 
    add_custom_target(link_${KERNEL_NAME}_hardware
      COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${SDAccel_XOCC} -l -t hw
      ${XOCC_COMMAND} ${KERNEL_NAME}_hw.xo -o ${KERNEL_NAME}_hw.xclbin) 
    add_custom_target(compile_${KERNEL_NAME}_hardware_emulation
      COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${SDAccel_XOCC} -c -t hw_emu -g
      ${XOCC_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw_emu.xo) 
    add_custom_target(link_${KERNEL_NAME}_hardware_emulation
      COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${SDAccel_XOCC} -l -t hw_emu -g
      ${XOCC_COMMAND} ${KERNEL_NAME}_hw_emu.xo -o ${KERNEL_NAME}_hw_emu.xclbin) 
  endif()
  add_executable(Run${KERNEL_NAME}.exe host/Run${KERNEL_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp)
  target_link_libraries(Run${KERNEL_NAME}.exe ${SDAccel_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  add_custom_target(run_${KERNEL_NAME}_hardware COMMAND ${CMAKE_CURRENT_BINARY_DIR}/Run${KERNEL_NAME}.exe hardware)
  add_custom_target(run_${KERNEL_NAME}_emulation COMMAND ${CMAKE_COMMAND} -E env XILINX_SDX=${SDACCEL_ROOT_DIR} XCL_EMULATION_MODE=hw_emu ${CMAKE_CURRENT_BINARY_DIR}/Run${KERNEL_NAME}.exe emulation)
  add_custom_target(run_${KERNEL_NAME}_simulation COMMAND ${CMAKE_COMMAND} -E env XCL_EMULATION_MODE=sw_emu XILINX_SDX=${SDACCEL_ROOT_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Run${KERNEL_NAME}.exe simulation)
endfunction()

add_subdirectory(example_0)
add_subdirectory(example_1)
add_subdirectory(example_2)
add_subdirectory(example_3)
add_subdirectory(example_4)
add_subdirectory(example_5)
