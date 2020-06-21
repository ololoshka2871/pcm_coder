#ifndef SAMPLEGENERATOR_H
#define SAMPLEGENERATOR_H

#include <random>
#include <vector>

#include "audioreader.h"

struct SampleGenerator {
  using i_samples_format = AudioSample<int16_t>;
  using o_samples_format = AudioSample<int16_t>;

  SampleGenerator(bool is_16_bit = false, bool use_dither = false);

  std::vector<o_samples_format> convert(const i_samples_format *in,
                                        size_t samples_count);

  o_samples_format convert(const i_samples_format &in);

private:
  std::default_random_engine generator;
  std::normal_distribution<float> distribution;

  bool is_14_bit;
  bool use_dither;

  int16_t random_ps1();
};

#endif // SAMPLEGENERATOR_H
