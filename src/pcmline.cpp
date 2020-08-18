#include <algorithm>
#include <cstring>

#include "PQ.h"
#include "crc16_ccitt.h"

#include "pcmline.h"

#define CACHED_RENDER 1

template <size_t size> struct PixelCachEntry {
  void assign(uint32_t v, uint8_t H) {
    std::memset(values, 0, size);
    for (uint32_t i = 0; i < size; ++i) {
      if (v & (1 << (size - i - 1))) {
        values[i] = H;
      }
    }
  }

  uint8_t values[size];
};

static constexpr size_t PixelCacheBits = PCMLine::BITS_PRE_COLUMN / 2;
static constexpr size_t CRCPixelCacheBits = sizeof(uint16_t) * CHAR_BIT / 2;

static PixelCachEntry<PixelCacheBits> PixelCache[1 << PixelCacheBits];
static PixelCachEntry<CRCPixelCacheBits> CRCPixelCache[1 << CRCPixelCacheBits];

static uint8_t CachedHValue = 0;

static void RebuildPixelCache(uint8_t H) {
  for (auto i = 0; i < (1 << PixelCacheBits); ++i) {
    PixelCache[i].assign(i, H);
  }
  for (auto i = 0; i < (1 << CRCPixelCacheBits); ++i) {
    CRCPixelCache[i].assign(i, H);
  }

  CachedHValue = H;
}

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
#if 1
  static const auto total_data_words =
      TOTAL_DATA_BITS_PRE_LINE / (sizeof(uint16_t) * CHAR_BIT);

  union {
    uint8_t bytes[total_data_words * sizeof(uint16_t) / sizeof(uint8_t)];
    uint16_t words[total_data_words];
  } data;

  for (auto i = 0; i < total_data_words; ++i) {
    data.words[i] = getDataWord(i);
  }

  return crc16_ccitt(data.bytes, sizeof(data.bytes));
#else

  std::array<uint8_t, 14> databytes;

  size_t b = 0;
  std::for_each(databytes.begin(), databytes.end(),
                [&b, this](auto &v) { v = this->getByte(b++); });

  return crc16_ccitt(databytes.data(), databytes.size());
#endif
}

uint8_t PCMLine::getFragment(uint32_t fragment) const {
  auto column = fragment / (BITS_PRE_COLUMN / PixelCacheBits);
  return (fragment & 1)
             ? data[column] & ((1 << PixelCacheBits) - 1)
             : (data[column] >> PixelCacheBits) & ((1 << PixelCacheBits) - 1);
}

uint8_t PCMLine::getCrcFragment(uint32_t crc_fragment) const {
  return (crc_fragment & 1) ? data[CRC_OFFSET] & ((1 << CRCPixelCacheBits) - 1)
                            : (data[CRC_OFFSET] >> CRCPixelCacheBits) &
                                  ((1 << CRCPixelCacheBits) - 1);
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

uint16_t PCMLine::getDataWord(uint8_t n) const {

  const auto first_elemet_index = n;
  const auto second_elemet_index = n + 1;

  const auto read_from_first_element = BITS_PRE_COLUMN - 2 * n;
  const auto read_from_second_element = 2 * (1 + n);

  const auto shift_first =
      sizeof(uint16_t) * CHAR_BIT - read_from_first_element;
  const auto shift_second = BITS_PRE_COLUMN - read_from_second_element;

  const auto mask_second = (1 << read_from_second_element) - 1;

  uint16_t r1 = (data[first_elemet_index] << shift_first);
  uint16_t r2 = (data[second_elemet_index] >> shift_second) & mask_second;

  uint16_t r = r1 | r2;
  return (r >> 8) | (r << 8);
}

void PCMLine::renderTo(uint8_t *pixelBuffer, uint8_t H) const {
#if CACHED_RENDER
  if (H != CachedHValue) {
    RebuildPixelCache(H);
  }

  for (auto fragment = 0; fragment < TOTAL_DATA_BITS_PRE_LINE / PixelCacheBits;
       ++fragment) {
    auto f = getFragment(fragment);
    std::memcpy(pixelBuffer, PixelCache[f].values, PixelCacheBits);
    pixelBuffer += PixelCacheBits;
  }

  for (auto crc_fragment = 0;
       crc_fragment <
       (TOTAL_BITS_PRE_LINE - TOTAL_DATA_BITS_PRE_LINE) / CRCPixelCacheBits;
       ++crc_fragment) {
    auto f = getCrcFragment(crc_fragment);
    std::memcpy(pixelBuffer, CRCPixelCache[f].values, CRCPixelCacheBits);
    pixelBuffer += CRCPixelCacheBits;
  }

#else
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
#endif
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
