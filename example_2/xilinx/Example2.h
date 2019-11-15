#pragma once

#include "hlslib/xilinx/Stream.h"
using hlslib::Stream;

constexpr int N = 32;
constexpr int M = 32;

void Stencil2D(float const *memory_in, float *memory_out);
void Example2(float const *memory_in, float *memory_out);
