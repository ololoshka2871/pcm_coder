#ifndef LINEGENERATORSTAGE_H
#define LINEGENERATORSTAGE_H

#include "abstractprocessingstage.h"

#include "samplegenerator.h"

#include "samplespack.h"

#include "pcmline.h"

struct LineGeneratorStage
    : public AbstractprocessingStage<SamplesPack, PCMLine> {
  LineGeneratorStage(bool mode14Bit = true, bool generateP = true,
                     bool generateQ = true);

  void Ressive(const SamplesPack &samples) override;

private:
  std::vector<SampleGenerator::o_samples_format> overflow;
  bool mode14Bit, generateP, generateQ;

  template <typename T> void write_overflow(T &it) {
    for (auto &v : overflow) {
      write_sample(it, v);
    }
  }

  template <typename T>
  void write_sample(T &it, const SampleGenerator::o_samples_format &sample) {
    *it++ = sample.L;
    *it++ = sample.R;
  }
};

#endif // LINEGENERATORSTAGE_H
