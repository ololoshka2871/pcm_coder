#ifndef PCMLINEGENERATOR_H
#define PCMLINEGENERATOR_H

#include "pcmline.h"

#include "samplegenerator.h"

#include "LockingQueue.h"

struct PCMLineGenerator {
  //  из присланного набора сеплов генерирует PCMLine кладет в очередь

  PCMLineGenerator(LockingQueue<PCMLine> &outputQueue);

  void input(const std::vector<SampleGenerator::o_samples_format> &samples);

  void flush();

private:
  std::vector<SampleGenerator::o_samples_format> overflow;
  LockingQueue<PCMLine> &outputQueue;

  void sendLine(const PCMLine &line) { outputQueue.push(line); }

  template <typename T>
  void write_sampe(T &it, const SampleGenerator::o_samples_format &sample) {
    *it++ = 0x0A; // sample.L;
    *it++ = 0xA0; // sample.R;
  }

  template <typename T> void write_overflow(T &it) {
    for (auto &v : overflow) {
      write_sampe(it, v);
    }
  }
};

#endif // PCMLINEGENERATOR_H
