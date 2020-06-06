#include <cmath>

#include "samplegenerator.h"

SampleGenerator::SampleGenerator(bool is_16_bit, bool use_dither) {}

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
  return o_samples_format{{static_cast<int16_t>(std::roundf(in.L * 0x7fff)),
                           static_cast<int16_t>(std::roundf(in.R * 0x7fff))}};
}
