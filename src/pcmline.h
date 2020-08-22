#ifndef PCMLINE_H
#define PCMLINE_H

#include <array>
#include <climits>
#include <cstdint>

struct PCMLine {
  // Хранит 6 (3 * 2) аудио-сеплов, может вычислить P и Q

  enum {
    TotalDataLRSamples = 3,                           // RL
    TotalChanelSamples = TotalDataLRSamples * 2,      // R / L
    TotalChanelSamplesWithP = TotalChanelSamples + 1, // RL RL RL P

    TotalDataPreLine = 2 * TotalDataLRSamples + 2,  // RL RL RL P Q
    TotalDataPreLineWithCRC = TotalDataPreLine + 1, // RL RL RL P Q CRC

    P_offset = TotalChanelSamples,
    Q_offset = P_offset + 1,

    CRC_OFFSET = Q_offset + 1,

    DATA_SAMPLES_COUNT = CRC_OFFSET,
    SAMPLES_COUNT = DATA_SAMPLES_COUNT + 1 // + CRC
  };

  static constexpr auto BITS_PRE_COLUMN = 14;
  static constexpr auto TOTAL_DATA_BITS_PRE_LINE =
      DATA_SAMPLES_COUNT * BITS_PRE_COLUMN;
  static constexpr auto TOTAL_BITS_PRE_LINE =
      TOTAL_DATA_BITS_PRE_LINE + sizeof(uint16_t) * CHAR_BIT;

  PCMLine() : data{} {}

  const uint16_t *iterator() const { return std::begin(data); }
  uint16_t *iterator() { return std::begin(data); }
  const uint16_t *pCRC() const { return &data[TotalDataPreLine]; }
  uint16_t *pCRC() { return &data[TotalDataPreLine]; }
  const uint16_t *pQ() const { return &data[Q_offset]; }
  uint16_t *pQ() { return &data[Q_offset]; }
  uint16_t *end() { return std::end(data); }

  uint16_t generateP() const;
  uint16_t generateQ() const;
  uint16_t generate16BitExtention() const;

  // only for 16 bit mode. Set main samples 14 bit, not 16
  void shiftMainDataAndP();

  uint16_t generateCRC() const;

  const uint16_t *getData() const { return data; }

  uint16_t &operator[](size_t sample) { return data[sample]; }

  virtual bool isEOF() const { return false; }

  inline bool getBit(size_t bit_n) const {
    auto column = bit_n / BITS_PRE_COLUMN;
    auto mask = (1u << (BITS_PRE_COLUMN - 1)) >> (bit_n % BITS_PRE_COLUMN);
    return !!(data[column] & mask);
  }

  inline bool getCRCBit(size_t bit_n) const {
    auto mask = (1u << (sizeof(uint16_t) * CHAR_BIT - 1)) >> bit_n;
    return !!(data[CRC_OFFSET] & mask);
  }

  uint8_t getFragment(uint32_t fragment) const;
  uint8_t getCrcFragment(uint32_t crc_fragment) const;

  uint8_t getByte(size_t byten) const;

  uint16_t getDataWord(uint8_t n) const;

  void renderTo(uint8_t *pixelBuffer, uint8_t H) const;

private:
  uint16_t data[TotalDataPreLineWithCRC];

  std::array<uint8_t, TotalChanelSamplesWithP * BITS_PRE_COLUMN>
  toBinArray(const uint16_t data[]) const;

  uint16_t QfromBinArray(std::array<uint8_t, BITS_PRE_COLUMN> &src) const;
};

struct PCMLineEof : public PCMLine {
  bool isEOF() const override { return true; }
};

#endif // PCMLINE_H
