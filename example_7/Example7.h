#pragma once

#include "hlslib/DataPack.h"

constexpr int N = 256;
constexpr int K = 256;
constexpr int M = 256;

constexpr int W = 4; 
constexpr int D = 8;
constexpr int TM = 32;

using Vec_t = hlslib::DataPack<float, W>;

static_assert(N % D == 0, "Size must be divisable by tile size");
static_assert(M % (TM * W) == 0, "Size must be divisable by tile size");

void Entry(float const a[], Vec_t const b[], Vec_t c[]);
