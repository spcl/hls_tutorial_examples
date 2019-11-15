#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "Example7.h"

void Reference(float const a[], float const b[], float c[]) {

  for (int n = 0; n < N; ++n) {
    for (int m = 0; m < M; ++m) {
      c[n * M + m] = 0;
      for (int k = 0; k < K; ++k) {
        c[n * M + m] += a[n * K + k] * b[k * M + m];
      }
    }
  }
}

int main() {

  std::vector<float> a(N * K);
  std::vector<float> b(K * M);
  std::vector<float> c(N * M);
  std::vector<float> c_ref(N * M, 0);

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(a.begin(), a.end(), [&](float &i) { i = dist(rng); });
  std::for_each(b.begin(), b.end(), [&](float &i) { i = dist(rng); });

  // Run simulation
  Entry(a.data(), reinterpret_cast<Vec_t const *>(&b[0]),
        reinterpret_cast<Vec_t *>(&c[0]));

  // Reference implementation for comparing the result
  Reference(a.data(), b.data(), c_ref.data());

  // Verify correctness
  for (int i = 0; i < N * M; ++i) {
    const auto diff = std::abs(c_ref[i] - c[i]);
    if (diff >= 1e-3) {
      std::cout << "Mismatch at (" << i / M << ", " << i % M << "): " << c[i]
                << " (should be " << c_ref[i] << ").\n";
      return 1;
    }
  }
  std::cout << "Test ran successfully.\n";

  return 0;
}
