
#include "PQ.h"

#include "pcmline.h"

PCMLine PCMLine::eof() {
  PCMLine line;
  line.isEof = true;
  return line;
}

uint16_t PCMLine::generateP() const {
  uint16_t P = 0;
  for (auto i = 0; i < TotalChanelSamples; ++i) {
    P ^= data[i];
  }
  return P;
}

uint16_t PCMLine::generateQ() const { return 2; }

std::pair<uint16_t, uint16_t> PCMLine::generatePQ() {
  auto binArray = toBinArray(data);

  std::array<uint8_t, BITS_PRE_COLUMN> Qarray;
  GenerateQ(binArray.data(), Qarray.data());

  uint16_t Q = QfromBinArray(Qarray);

  return {generateP(), Q};
}

uint16_t PCMLine::generateCRC() const { return 0x0FF0; }

uint16_t
PCMLine::QfromBinArray(std::array<uint8_t, PCMLine::BITS_PRE_COLUMN> &src) {
  uint16_t res = 0;

  int bit = 13;
  for (auto it = src.cbegin(); it != src.cend(); ++it, --bit) {
    if (*it) {
      res |= 1 << bit;
    }
  }

  return res;
}

std::array<uint8_t, PCMLine::TotalChanelSamplesWithP * PCMLine::BITS_PRE_COLUMN>
PCMLine::toBinArray(uint16_t data[]) {
  std::array<uint8_t,
             PCMLine::TotalChanelSamplesWithP * PCMLine::BITS_PRE_COLUMN>
      res;

  int bit = 0;
  for (auto it = res.begin(); it != res.end(); ++it, ++bit) {
    *it = getBit(bit);
  }

  return res;
}
