#ifndef PCMLINE_H
#define PCMLINE_H

#include <array>
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

    DATA_SAMPLES_COUNT = Q_offset + 1,
    SAMPLES_COUNT = DATA_SAMPLES_COUNT + 1 // + CRC
  };

  static constexpr auto BITS_PRE_COLUMN = 14;
  static constexpr auto TOTAL_DATA_BITS_PRE_LINE =
      DATA_SAMPLES_COUNT * BITS_PRE_COLUMN;
  static constexpr auto TOTAL_BITS_PRE_LINE = SAMPLES_COUNT * BITS_PRE_COLUMN;

  PCMLine() : data{}, isEof{false} {}

  static PCMLine eof();

  uint16_t *iterator() { return std::begin(data); }
  uint16_t *pCRC() { return &data[TotalDataPreLine]; }
  uint16_t *end() { return std::end(data); }

  uint16_t generateP() const;
  uint16_t generateQ() const;
  std::pair<uint16_t, uint16_t> generatePQ();

  uint16_t generateCRC() const;

  const uint16_t *getData() const { return data; }

  uint16_t &operator[](size_t sample) { return data[sample]; }

  bool isEOF() const { return isEof; }

  inline bool getBit(size_t bit_n) const {
    auto column = bit_n / BITS_PRE_COLUMN;
    auto mask = (1u << 13) >> (bit_n % BITS_PRE_COLUMN);
    return !!(data[column] & mask);
  }

private:
  uint16_t data[TotalDataPreLineWithCRC];
  bool isEof;

  std::array<uint8_t, TotalChanelSamplesWithP * BITS_PRE_COLUMN>
  toBinArray(uint16_t data[TotalDataPreLineWithCRC]);

  uint16_t QfromBinArray(std::array<uint8_t, BITS_PRE_COLUMN> &src);
};

#endif // PCMLINE_H
