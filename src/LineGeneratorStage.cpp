#include "LineGeneratorStage.h"

LineGeneratorStage::LineGeneratorStage(bool mode14Bit, bool generateP,
                                       bool generateQ)
    : overflow{}, mode14Bit{mode14Bit}, generateP{generateP}, generateQ{
                                                                  generateQ} {}

void LineGeneratorStage::Ressive(const SamplesPack &samples) {
  if (samples.eof()) {
    Send(PCMLineEof{});
    return;
  }

  PCMLine line;

  const auto line_start = line.iterator();
  auto itl = line_start;
  auto itl_data_end = line_start + PCMLine::TotalChanelSamples;

  write_overflow(itl);

  auto start = samples.data.cbegin();
  auto ovf_count = (samples.data.size() + overflow.size()) % PCMLine::TotalDataLRSamples;
  auto end = samples.data.end() - ovf_count;

  overflow.assign(end, samples.data.end()); // replace overflow

  for (auto it = start; it != end; ++it) { // it - итератор пары
    write_sample(itl, *it);

    if (itl == itl_data_end) {
        if (generateP) {
            *itl = line.generateP();
        }
        ++itl;
        if (generateQ && mode14Bit) // skip generate Q if 16 bit mode
        {
            *itl = line.generateQ();
        }
        Send(line);

        itl = line_start;
    }
  }
}
