cmake_minimum_required(VERSION 3.0)
project(hls_tutorial_examples)

# Options
set(XILINX_PLATFORM "xilinx_u250_gen3x16_xdma_3_1_202020_1" CACHE STRING "Target vitis platform.")
set(INTEL_FPGA_BOARD "s5_ref" CACHE STRING "Target board for aoc.")

# Include find-scripts from hlslib
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/hlslib/cmake)

find_package(Threads REQUIRED)
find_package(Vitis)
find_package(IntelFPGAOpenCL)

# Set up compiling Xilinx codes
include_directories(${CMAKE_SOURCE_DIR}/hlslib/include)
# Required to link Intel OpenCL
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

enable_testing()

# Sets up a synthesis target for the given kernel
function(xilinx_synthesis_target KERNEL_NAME REPORT_MODULE EXTRA_FILES)
  string(TOLOWER ${KERNEL_NAME} KERNEL_NAME_LOWER)
  foreach(CODE_FILE ${KERNEL_NAME}.cpp ${EXTRA_FILES})
    set(KERNEL_SRC ${KERNEL_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/xilinx/${CODE_FILE})
  endforeach()
  add_vitis_kernel(${KERNEL_NAME_LOWER}
                   FILES ${KERNEL_SRC}
                   KERNEL ${KERNEL_NAME}
                   INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/hlslib/include
                                ${CMAKE_CURRENT_SOURCE_DIR}/xilinx)
  add_vitis_program(${KERNEL_NAME_LOWER} ${XILINX_PLATFORM})
  if(Vitis_USE_VITIS_HLS)
    set(REPORT_NAME ${KERNEL_NAME})
  else()
    set(REPORT_NAME ${REPORT_MODULE})
  endif()
  add_custom_command(
    TARGET synthesize_${KERNEL_NAME_LOWER}
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/${KERNEL_NAME_LOWER}/${PROGRAM_PLATFORM_PART}/syn/report/${REPORT_NAME}_csynth.rpt
            ${CMAKE_CURRENT_BINARY_DIR}/report_${KERNEL_NAME_LOWER}.rpt)
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
