#ifndef PCMFRAME_H
#define PCMFRAME_H

#include <climits>
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

#include "MyTwoDimArray.h"

struct PCMFrame14 {
  enum FrameType { PAL, NTSC };
  enum Chanel { Left = 0, Right = 1 };

  //! L R |  L R  | L R | P | Q | CRC
  static constexpr auto pcm_data_columns = 3;
  static constexpr auto pcm_P_column = 6;
  static constexpr auto pcm_Q_column = 7;
  static constexpr auto pcm_CRC_offset = 8 * 14;

  static constexpr auto pcm_shift_rows = 16;

  static constexpr auto WIDTH = 139;

  static constexpr size_t initial_data_x_offset = 5;

  static constexpr size_t stairs_last_shift =
      pcm_shift_rows * pcm_data_columns * 2;

  static constexpr auto frame_bit_width = 14;

  static uint8_t gray_lvl;
  static uint8_t white_lvl;

  static PCMFrame14 newFrame(PCMFrame14::FrameType frameType);

  PCMFrame14(const PCMFrame14 &lr) = default;

  void setAudioSample(Chanel chanel, uint32_t number, uint16_t sample_data);
  uint16_t getAudioSample(Chanel chanel, uint32_t number, int newParameter = 2);

  uint16_t generateP(uint32_t startrow);
  uint16_t generateQ(uint32_t startrow);

  uint16_t generateCRC(uint32_t row);

  FrameType Type() const { return frameType; }
  std::pair<size_t, size_t> getSize() const;

  void writeFrameTo(uint8_t *dest);

private:
  void write_data(size_t x, size_t y, std::vector<uint8_t> data);
  size_t column2offset(size_t raw_offset) const {
    return raw_offset + initial_data_x_offset;
  }

  std::vector<uint8_t> SampletoPixels(uint16_t val);
  uint16_t SampleFromPixels(const uint8_t *pixels);

  size_t calc_sample_line(Chanel chanel, uint32_t number) const {
    // L  R  L  R  L  R  | P  Q
    // 00 16 32 48 64 80 | 96 112
    // 01 17 33 49 65 81 | 97 113

    // TODO test!
    return ((chanel * 2 + number) * pcm_shift_rows) % stairs_last_shift +
           (chanel * 2 + number) /
               (pcm_data_columns * 2); // +1 каждые 6 семплов
  }

  size_t calc_sample_column(Chanel chanel, uint32_t number) const {
    return number / pcm_data_columns + chanel;
  }

  template <typename T> T &read_data(size_t x, size_t y) {
    return *reinterpret_cast<T *>(&pixel_accessor.getLine(y)[x]);
  }

  uint16_t getAudioSample(PCMFrame14::Chanel chanel, uint32_t number);

  PCMFrame14(FrameType frameType);

  std::vector<uint8_t> pixels;
  FrameType frameType;
  myTwoDimArray<uint8_t> pixel_accessor;

  static std::unique_ptr<PCMFrame14> clear_frame;
};

#endif // PCMFRAME_H
