#ifndef PCMLINE_H
#define PCMLINE_H

#include <cstdint>
#include <vector>

struct PCMLine {
  // Хранит 6 (3 * 2) аудио-сеплов, может вычислить P и Q

  enum {
    TotalDataLRSamples = 3,                        // RL
    TotalChanelSamples = TotalDataLRSamples * 2,   // R / L
    TotalDataPreLine = 2 * TotalDataLRSamples + 2, // RL RL RL P Q

    P_offset = TotalChanelSamples,
    Q_offset = P_offset + 1,
  };

  PCMLine() : data(TotalDataPreLine), isEof{false} {}

  static PCMLine eof();

  std::vector<uint16_t>::iterator iterator() { return data.begin(); }

  uint16_t generateP() const;
  uint16_t generateQ() const;
  std::pair<uint16_t, uint16_t> generatePQ();

  const std::vector<uint16_t> &getData() const { return data; }

  bool isEOF() const { return isEof; }

private:
  std::vector<uint16_t> data;
  bool isEof;
};

#endif // PCMLINE_H
