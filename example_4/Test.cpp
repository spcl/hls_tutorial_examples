#include <algorithm>
#include <random>
#include <vector>

#include "Example4.h"

void Reference(float const in[], float out[]) {
  float tmp[N];
  std::copy(in, in + N, tmp);
  for (int d = 0; d < D; ++d) {
    auto read = (d % 2 == 0) ? tmp : out;
    auto write = (d % 2 == 0) ? out : tmp;
    for (int i = d; i < N - d; ++i) {
      write[i] = 0.3333 * (read[i - 1] + read[i] + read[i + 1]);
    }
  }
  if (D % 2 == 0) {
    std::copy(tmp, tmp + N, out);
  }
  for (int i = 0; i < D; ++i) {
    out[i] = 0;
    out[N - D + i] = 0;
  }
}

int main() {
  std::vector<float> in(N);

  std::random_device rd;
  std::default_random_engine rng;
  std::uniform_real_distribution<float> dist;
  std::for_each(in.begin(), in.end(), [&](float &i) { i = dist(rng); });

  std::vector<float> out_ref(N);
  std::vector<float> out_res(N);

  Entry(in.data(), out_res.data());
  Reference(in.data(), out_ref.data());

  for (int i = 0; i < N; ++i) {
    const auto diff = std::abs(out_ref[i] - out_res[i]);
    if (diff > 1e-4) {
      std::cerr << "Mismatch at " << i << ": " << out_res[i]
                << " (should be " << out_ref[i] << ")." << std::endl;
      return 1;
    }
  }
  std::cout << "Test ran successfully.\n";

  return 0;
}
