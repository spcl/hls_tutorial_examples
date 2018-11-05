#pragma once

#include "hlslib/Stream.h"
using hlslib::Stream;

constexpr int N = 32;
constexpr int M = 32;

void Stencil2D(float const *memory_in, float *memory_out);
void Entry(float const *memory_in, float *memory_out);
