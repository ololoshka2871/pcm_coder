#include <algorithm>

#include "PQ.h"
#include "crc16_ccitt.h"

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

uint16_t PCMLine::generateQ() const {
  auto binArray = toBinArray(data);

  std::array<uint8_t, BITS_PRE_COLUMN> Qarray;
  GenerateQ(binArray.data(), Qarray.data());

  return QfromBinArray(Qarray);
}

uint16_t PCMLine::generate16BitExtention() const {
  static constexpr uint16_t bit01 = (1 << 1) | (1 << 0);

  uint16_t res = 0;
  for (auto i = 0; i < TotalChanelSamplesWithP; ++i) {
    uint16_t v = data[i] & bit01;
    auto shift = (TotalChanelSamples - i) * 2;
    res |= v << shift;
  }
  return res;
}

void PCMLine::shiftMainDataAndP() {
  for (auto i = 0; i < TotalChanelSamplesWithP; ++i) {
    data[i] >>= 2;
  }
}

uint16_t PCMLine::generateCRC() const {
  std::array<uint8_t, 14> databytes;

  size_t b = 0;
  std::for_each(databytes.begin(), databytes.end(),
                [&b, this](auto &v) { v = this->getByte(b++); });

  return crc16_ccitt(databytes.data(), databytes.size());
}

uint8_t PCMLine::getByte(size_t byten) const {
  auto start_bit_n = byten * CHAR_BIT;
  auto end_bit_n = start_bit_n + CHAR_BIT;

  uint8_t res = 0;
  for (int bit = 7; start_bit_n < end_bit_n; --bit, ++start_bit_n) {
    if (getBit(start_bit_n)) {
      res |= 1 << bit;
    }
  }

  return res;
}

void PCMLine::renderTo(uint8_t *pixelBuffer, uint8_t H) const {
  for (auto bitn = 0; bitn < TOTAL_DATA_BITS_PRE_LINE; ++bitn) {
    if (getBit(bitn)) {
      *pixelBuffer = H;
    }
    ++pixelBuffer;
  }

  auto crc_but_n = 0;
  for (auto bitn = TOTAL_DATA_BITS_PRE_LINE; bitn < TOTAL_BITS_PRE_LINE;
       ++bitn, ++crc_but_n) {
    if (getCRCBit(crc_but_n)) {
      *pixelBuffer = H;
    }
    ++pixelBuffer;
  }
}

uint16_t PCMLine::QfromBinArray(
    std::array<uint8_t, PCMLine::BITS_PRE_COLUMN> &src) const {
  uint16_t res = 0;

  int bit = 13;
  std::for_each(src.cbegin(), src.cend(), [&bit, &res](auto basByte) {
    if (basByte) {
      res |= 1 << bit;
    }
    --bit;
  });

  return res;
}

std::array<uint8_t, PCMLine::TotalChanelSamplesWithP * PCMLine::BITS_PRE_COLUMN>
PCMLine::toBinArray(const uint16_t data[]) const {
  std::array<uint8_t,
             PCMLine::TotalChanelSamplesWithP * PCMLine::BITS_PRE_COLUMN>
      res;

  int bit = 0;
  std::for_each(res.begin(), res.end(),
                [&bit, this](auto &rbyte) { rbyte = getBit(bit++); });

  return res;
}
