#include "pcmlinegenerator.h"

PCMLineGenerator::PCMLineGenerator(LockingQueue<PCMLine> &outputQueue)
    : outputQueue(outputQueue) {}

void PCMLineGenerator::input(
    const std::vector<SampleGenerator::o_samples_format> &samples) {

  PCMLine line;

  const auto line_start = line.iterator();
  auto itl = line_start;
  auto itl_data_end = itl + PCMLine::TotalChanelSamples;

  write_overflow(itl);

  auto start = samples.cbegin();
  auto end = samples.end() - samples.size() % PCMLine::TotalDataLRSamples;

  overflow.assign(end, samples.end()); // replace overflow

  for (auto it = start; it != end; ++it) {
    if (itl == itl_data_end) {
      *itl++ = line.generateP();
      *itl = line.generateQ();

      sendLine(line);

      itl = line_start;
    } else {
      write_sample(itl, *it);
    }
  }
}

void PCMLineGenerator::flush() { sendLine(PCMLine::eof()); }
