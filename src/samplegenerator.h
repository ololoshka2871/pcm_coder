#ifndef SAMPLEGENERATOR_H
#define SAMPLEGENERATOR_H

#include <vector>

#include "audioreader.h"

struct SampleGenerator {
  using i_samples_format = AudioSample<float>;
  using o_samples_format = AudioSample<int16_t>;

  SampleGenerator(bool is_16_bit = false, bool use_dither = false);

  std::vector<o_samples_format> convert(const i_samples_format *in,
                                        size_t samples_count);

  o_samples_format convert(const i_samples_format &in);

private:
  bool is_16_bit;
  bool use_dither;
};

#endif // SAMPLEGENERATOR_H
