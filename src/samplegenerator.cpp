#include <climits>
#include <cmath>
#include <limits>

#include "samplegenerator.h"

template <size_t shift> static int16_t applyLimits(int32_t sample) {
  static const auto min = std::numeric_limits<int16_t>::min() >>
                          (sizeof(int16_t) * CHAR_BIT - shift);
  static const auto max = std::numeric_limits<int16_t>::max() >>
                          (sizeof(int16_t) * CHAR_BIT - shift);

  if (sample > max) {
    return max;
  } else if (sample < min) {
    return min;
  }

  return sample;
}

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
  auto L = static_cast<int32_t>(in.L * 0x7fff);
  auto R = static_cast<int32_t>(in.R * 0x7fff);

  if (is_14_bit) {
    L >>= 2;
    R >>= 2;
    if (use_dither) {
      L += random_ps1();
      R += random_ps1();
    }
    return AudioSample<int16_t>{applyLimits<14>(L), applyLimits<14>(R)};
  } else {
    return AudioSample<int16_t>{applyLimits<16>(L), applyLimits<16>(R)};
  }
}

int16_t SampleGenerator::random_ps1() {
  auto v = distribution(generator);
  return static_cast<int16_t>(v);
}
