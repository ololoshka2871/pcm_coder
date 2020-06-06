#include <cmath>

#include "samplegenerator.h"

SampleGenerator::SampleGenerator(bool is_14_bit, bool use_dither)
    : generator{}, distribution{-1.0f, 1.0f}, is_14_bit(is_14_bit),
      use_dither(use_dither) {}

std::vector<SampleGenerator::o_samples_format>
SampleGenerator::convert(const i_samples_format *in, size_t samples_count) {
  std::vector<o_samples_format> res;
  res.reserve(samples_count);

  for (auto i = 0; i < samples_count; ++i) {
    res.push_back(convert(in[i]));
  }

  return res;
}

SampleGenerator::o_samples_format
SampleGenerator::convert(const i_samples_format &in) {
  AudioSample<int16_t> r{static_cast<int16_t>(std::roundf(in.L * 0x7fff)),
                         static_cast<int16_t>(std::roundf(in.R * 0x7fff))};
  if (is_14_bit) {
    r.L >>= 2;
    r.R >>= 2;
    if (use_dither) {
      r.L += random_ps1();
      r.R += random_ps1();
    }
  }

  return r;
}

int16_t SampleGenerator::random_ps1() {
  auto v = distribution(generator);
  return static_cast<int16_t>(v);
}
