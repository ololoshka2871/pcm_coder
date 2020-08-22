#include <iostream>

#include "pcmlinegenerator.h"

/*
PCMLineGenerator::PCMLineGenerator(LockingQueue<PCMLine> &outputQueue)
    : outputQueue(outputQueue), mode14Bit{true}, generateP{true}, generateQ{
                                                                      true} {}
                                                                      */

PCMLineGenerator::PCMLineGenerator(
    PCMFrmageManager::ILineProcessor *line_processor)
    : line_processor{line_processor}, mode14Bit{true}, generateP{true},
      generateQ{true} {}

void PCMLineGenerator::input(
    const std::vector<SampleGenerator::o_samples_format> &samples) {

  PCMLine line;

  const auto line_start = line.iterator();
  auto itl = line_start;
  auto itl_data_end = line_start + PCMLine::TotalChanelSamples;

  write_overflow(itl);

  auto start = samples.cbegin();
  auto ovf_count = samples.size() % PCMLine::TotalDataLRSamples;
  auto end = samples.end() - ovf_count;

  overflow.assign(end, samples.end()); // replace overflow

  for (auto it = start; it != end; ++it) { // it - итератор пары
    if (itl == itl_data_end) {
      if (generateP) {
        *itl = line.generateP();
      }
      ++itl;
      if (generateQ && mode14Bit) // skip generate Q if 16 bit mode
      {
        *itl = line.generateQ();
      }

      sendLine(line);

      itl = line_start;
    }

    write_sample(itl, *it);
  }
}

void PCMLineGenerator::flush() {
  PCMLineEof eof;
  sendLine(eof);
}

PCMLineGenerator &PCMLineGenerator::set14BitMode(bool mode14Bit) {
  this->mode14Bit = mode14Bit;
  return *this;
}

PCMLineGenerator &PCMLineGenerator::setGenerateP(bool generateP) {
  this->generateP = generateP;
  if (!generateP) {
    generateQ = false;
  }
  return *this;
}

PCMLineGenerator &PCMLineGenerator::setGenerateQ(bool generateQ) {
  this->generateQ = generateP & generateQ;
  return *this;
}
