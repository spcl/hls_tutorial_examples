#pragma once

#include "hlslib/xilinx/Stream.h"
using hlslib::Stream;

constexpr int N = 1024;
constexpr int D = 8;

void Entry(float const *in, float *out);
