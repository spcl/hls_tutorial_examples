cmake_minimum_required(VERSION 3.0)
project(hls_tutorial_examples)

# Options
set(XILINX_PART_NAME "xcvu9p-flgb2104-2-i" CACHE STRING "Part name for HLS.")
set(XILINX_DSA_NAME "xilinx_vcu1525_dynamic_5_1" CACHE STRING "DSA string for xocc.")
set(INTEL_FPGA_BOARD "s5_ref" CACHE STRING "Target board for aoc.")

# Include find-scripts from hlslib
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/hlslib/cmake)

find_package(Threads REQUIRED)
find_package(Vitis)
find_package(IntelFPGAOpenCL)

# Set up compiling Xilinx codes
set(XILINX_SYNTHESIS_FLAGS "-DHLSLIB_SYNTHESIS -DHLSLIB_XILINX -std=c++11 -I${CMAKE_SOURCE_DIR}/hlslib/include -I.")
include_directories(${CMAKE_SOURCE_DIR}/hlslib/include)
# Required to link Intel OpenCL
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

enable_testing()

# Sets up a synthesis target for the given kernel
function(xilinx_synthesis_target KERNEL_NAME ADDITIONAL_FLAGS REPORT_MODULE EXTRA_FILES)
  if(Vitis_FOUND)
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
    if(Vitis_USE_VITIS_HLS)
      set(REPORT_NAME ${KERNEL_NAME})
    else()
      set(REPORT_NAME ${REPORT_MODULE})
    endif()
    add_custom_target(synthesize_${KERNEL_NAME_LOWER}
      COMMAND ${Vitis_HLS} -f Synthesize${KERNEL_NAME}.tcl 
      POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/${HLSLIB_PROJECT_NAME}/${HLSLIB_PART_NAME}/syn/report/${REPORT_NAME}_csynth.rpt
    ${CMAKE_CURRENT_BINARY_DIR}/report_${KERNEL_NAME_LOWER}.rpt) 
  endif()
endfunction()

# Builds a testbench that can run Vivado HLS C++ kernels in software simulation
function(xilinx_testbench CPP_FILES KERNEL_NAME)
  if(Vitis_FOUND)
    string(REPLACE " " ";" CPP_FILES "${CPP_FILES}")
    set(CODE_FILES)
    foreach(CODE_FILE ${KERNEL_NAME}.cpp ${CPP_FILES})
      set(CODE_FILES ${CODE_FILES}
          ${CMAKE_CURRENT_SOURCE_DIR}/xilinx/${CODE_FILE})
    endforeach()
    add_executable(Test${KERNEL_NAME}Xilinx ${CODE_FILES})
    target_include_directories(Test${KERNEL_NAME}Xilinx PUBLIC ${Vitis_INCLUDE_DIRS}) 
    target_link_libraries(Test${KERNEL_NAME}Xilinx ${CMAKE_THREAD_LIBS_INIT}
                          ${Vitis_LIBRARIES})
    add_test(Test${KERNEL_NAME}Xilinx Test${KERNEL_NAME}Xilinx)
  endif()
endfunction()

# Sets up a Vitis kernel build target for the given kernel
function(sdaccel_target KERNEL_NAME ADDITIONAL_FLAGS)
  if(Vitis_FOUND)
    set(XILINX_SYNTHESIS_FLAGS_FLAGS "${XILINX_SYNTHESIS_FLAGS} ${ADDITIONAL_FLAGS}")
    set(VPP_COMMAND
      -s
      -O3
      ${MM_VPP_FLAGS}
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
    if(((${Vitis_MAJOR_VERSION} LESS 2018) AND (${Vitis_MINOR_VERSION} LESS 3)) OR ${Vitis_MAJOR_VERSION} LESS 2017)
      add_custom_target(build_${KERNEL_NAME}_hardware
        COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${Vitis_VPP} -t hw
        ${VPP_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw.xclbin) 
      add_custom_target(build_${KERNEL_NAME}_hardware_emulation
        COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${Vitis_VPP} -t hw_emu -g
        ${VPP_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw_emu.xclbin) 
    else()
      add_custom_target(compile_${KERNEL_NAME}_hardware
        COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${Vitis_VPP} -c -t hw
        ${VPP_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw.xo) 
      add_custom_target(link_${KERNEL_NAME}_hardware
        COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${Vitis_VPP} -l -t hw
        ${VPP_COMMAND} ${KERNEL_NAME}_hw.xo -o ${KERNEL_NAME}_hw.xclbin) 
      add_custom_target(compile_${KERNEL_NAME}_hardware_emulation
        COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${Vitis_VPP} -c -t hw_emu -g
        ${VPP_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp -o ${KERNEL_NAME}_hw_emu.xo) 
      add_custom_target(link_${KERNEL_NAME}_hardware_emulation
        COMMAND XILINX_PATH=${CMAKE_CURRENT_BINARY_DIR} ${Vitis_VPP} -l -t hw_emu -g
        ${VPP_COMMAND} ${KERNEL_NAME}_hw_emu.xo -o ${KERNEL_NAME}_hw_emu.xclbin) 
    endif()
    add_executable(Run${KERNEL_NAME}.exe host/Run${KERNEL_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/kernels/${KERNEL_NAME}.cpp)
    target_include_directories(Run${KERNEL_NAME}.exe ${Vitis_INCLUDE_DIRS}) 
    target_link_libraries(Run${KERNEL_NAME}.exe ${Vitis_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    add_custom_target(run_${KERNEL_NAME}_hardware COMMAND ${CMAKE_CURRENT_BINARY_DIR}/Run${KERNEL_NAME}.exe hardware)
    add_custom_target(run_${KERNEL_NAME}_emulation COMMAND ${CMAKE_COMMAND} -E env XILINX_SDX=${SDACCEL_ROOT_DIR} XCL_EMULATION_MODE=hw_emu ${CMAKE_CURRENT_BINARY_DIR}/Run${KERNEL_NAME}.exe emulation)
    add_custom_target(run_${KERNEL_NAME}_simulation COMMAND ${CMAKE_COMMAND} -E env XCL_EMULATION_MODE=sw_emu XILINX_SDX=${SDACCEL_ROOT_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Run${KERNEL_NAME}.exe simulation)
  endif()
endfunction()

# Generate report for Intel
function(intel_synthesis_target KERNEL_NAME)
  if(IntelFPGAOpenCL_FOUND)
    string(TOLOWER ${KERNEL_NAME} KERNEL_NAME_LOWER)
    add_custom_command(
        OUTPUT ${KERNEL_NAME}/reports/report.html
        COMMAND ${IntelFPGAOpenCL_AOC} -board=${INTEL_FPGA_BOARD}
          -v -rtl
          -report ${CMAKE_CURRENT_SOURCE_DIR}/intel/${KERNEL_NAME}.cl
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intel/${KERNEL_NAME}.cl) 
    add_custom_target(synthesize_${KERNEL_NAME_LOWER}_intel
                      DEPENDS ${KERNEL_NAME}/reports/report.html) 
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${KERNEL_NAME}.aocx
        COMMAND ${IntelFPGAOpenCL_AOC} -board=${INTEL_FPGA_BOARD}
        -v -march=emulator
        -report ${CMAKE_CURRENT_SOURCE_DIR}/intel/${KERNEL_NAME}.cl
        -o ${CMAKE_CURRENT_BINARY_DIR}/${KERNEL_NAME}.aocx
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/intel/${KERNEL_NAME}.cl) 
    add_custom_target(kernel_${KERNEL_NAME_LOWER}_intel DEPENDS 
                      ${CMAKE_CURRENT_BINARY_DIR}/${KERNEL_NAME}.aocx)
  endif()
endfunction()

# Builds a testbench that can run Intel FPGA OpenCL kernels in emulation 
function(intel_testbench CPP_FILES KERNEL_NAME)
  if(IntelFPGAOpenCL_FOUND)
    string(TOLOWER ${KERNEL_NAME} KERNEL_NAME_LOWER)
    string(REPLACE " " ";" CPP_FILES "${CPP_FILES}")
    set(CODE_FILES)
    foreach(CODE_FILE ${CPP_FILES})
      set(CODE_FILES ${CODE_FILES}
          ${CMAKE_CURRENT_SOURCE_DIR}/intel/${CODE_FILE})
    endforeach()
    add_executable(Test${KERNEL_NAME}Intel ${CODE_FILES})
    target_include_directories(Test${KERNEL_NAME}Intel PUBLIC ${IntelFPGAOpenCL_INCLUDE_DIRS})
    target_link_libraries(Test${KERNEL_NAME}Intel ${CMAKE_THREAD_LIBS_INIT}
                          ${IntelFPGAOpenCL_LIBRARIES})
    add_test(NAME Test${KERNEL_NAME}Intel COMMAND ${CMAKE_COMMAND} -E env
             CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1
             ${CMAKE_CURRENT_BINARY_DIR}/Test${KERNEL_NAME}Intel
             ${CMAKE_CURRENT_BINARY_DIR}/${KERNEL_NAME}.aocx)
    add_dependencies(Test${KERNEL_NAME}Intel
                     kernel_${KERNEL_NAME_LOWER}_intel)
  endif()
endfunction()


add_subdirectory(example_0)
add_subdirectory(example_1)
add_subdirectory(example_2)
add_subdirectory(example_3)
add_subdirectory(example_4)
add_subdirectory(example_5)
add_subdirectory(example_6)
add_subdirectory(example_7)
