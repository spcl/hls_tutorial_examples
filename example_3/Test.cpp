#include <algorithm>
#include <iostream>
#include <vector>
#include <random>

#include "Example3.h" // Defines N and M

void Reference(float const in[], float out[]) {
  for (int i = 1; i < N - 1; ++i) {
    for (int j = 0; j < M; ++j) {
      out[i * M + j] =
          0.3333 * (in[(i - 1) * M + j] + in[i * M + j] + in[(i + 1) * M + j]);
    }
  }
}

int main() {
  std::vector<float> in(N*M); 

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(in.begin(), in.end(), [&](float &i) { i = dist(rng); });

  std::vector<float> out_ref(N*M);
  std::vector<float> out_res(N*M);

  Entry(in.data(), out_res.data());
  Reference(in.data(), out_ref.data());

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j) {
      const auto diff = std::abs(out_ref[i*M + j] - out_res[i*M + j]);
      if (diff > 1e-4) {
        std::cerr << "Mismatch at (" << i << ", " << j
                  << "): " << out_res[i * M + j] << " (should be "
                  << out_ref[i * M + j] << ")." << std::endl;
        return 1;
      }
    }
  }
  std::cout << "Test ran successfully.\n";

  return 0;
}
