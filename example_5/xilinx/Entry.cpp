#include "Example5.h"

void Example5(double const A[], double const B[], double C[]) {
  #pragma HLS INTERFACE m_axi port=A bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=B bundle=gmem1 offset=slave
  #pragma HLS INTERFACE m_axi port=C bundle=gmem2 offset=slave
  #pragma HLS INTERFACE s_axilite port=A bundle=control
  #pragma HLS INTERFACE s_axilite port=B bundle=control
  #pragma HLS INTERFACE s_axilite port=C bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  MatrixMultiplication(A, B, C);
}

void Example5_Reordered(double const A[], double const B[], double C[]) {
  #pragma HLS INTERFACE m_axi port=A bundle=gmem0 offset=slave
  #pragma HLS INTERFACE m_axi port=B bundle=gmem1 offset=slave
  #pragma HLS INTERFACE m_axi port=C bundle=gmem2 offset=slave
  #pragma HLS INTERFACE s_axilite port=A bundle=control
  #pragma HLS INTERFACE s_axilite port=B bundle=control
  #pragma HLS INTERFACE s_axilite port=C bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  MatrixMultiplication(A, B, C);
}
