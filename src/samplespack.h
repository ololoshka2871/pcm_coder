#ifndef SAMPLESPACK_H
#define SAMPLESPACK_H

#include <vector>

#include "samplegenerator.h"

struct SamplesPack {
  SamplesPack(std::vector<SampleGenerator::o_samples_format> &&data)
      : data{std::move(data)} {}

  virtual ~SamplesPack() = default;

  virtual bool eof() const { return false; }

  std::vector<SampleGenerator::o_samples_format> data;

protected:
  SamplesPack() {}
};

struct SamplesPackEof : public SamplesPack {
  SamplesPackEof() : SamplesPack{} {}

  bool eof() const override { return true; }
};

#endif // SAMPLESPACK_H
