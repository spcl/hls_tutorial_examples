#pragma once

#include "hlslib/xilinx/DataPack.h"

// Matrix dimensions
constexpr int N = 1024;
constexpr int M = N;
constexpr int K = N;

// Replication factors in depth (D) and width (W)
constexpr int D = 4;
constexpr int W = 4;

using Vec_t = hlslib::DataPack<float, W>;

void MatrixMultiplication(const float A[], const float B[], float C[]);
void MatrixMultiplication(const float A[], const Vec_t B[], Vec_t C[]);
void Example6(float const A[], float const B[], float C[]);
void Example6_Vectorized(float const A[], Vec_t const B[], Vec_t C[]);
