#pragma once

#include "hlslib/xilinx/DataPack.h"

constexpr int N = 1024;
constexpr int K = 1024;
constexpr int M = 1024;

constexpr int W = 4;
constexpr int D = 4;
constexpr int TM = 256;

using Vec_t = hlslib::DataPack<float, W>;

static_assert(N % D == 0, "Size must be divisable by tile size");
static_assert(M % (TM * W) == 0, "Size must be divisable by tile size");

void Example7(float const a[], Vec_t const b[], Vec_t c[]);
