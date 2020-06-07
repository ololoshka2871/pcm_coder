#include <array>
#include <cassert>
#include <cstring>

#include "pcmframe14.h"

std::unique_ptr<PCMFrame14> PCMFrame14::clear_frame;

uint8_t PCMFrame14::gray_lvl = 128;
uint8_t PCMFrame14::white_lvl = 255;

PCMFrame14 PCMFrame14::newFrame(FrameType frameType) {
  if (clear_frame == nullptr && clear_frame->frameType != frameType) {
    clear_frame.reset(new PCMFrame14{frameType});

    std::array<uint8_t, WIDTH> clear_pattern;
    clear_pattern[1] = clear_pattern[3] = gray_lvl;
    std::memset(clear_pattern.end() - 5, white_lvl, 4);
  }
  return *clear_frame;
}

std::pair<size_t, size_t> PCMFrame14::getSize() const {
  return std::make_pair<size_t, size_t>(
      WIDTH, (frameType == PAL) ? PAL_HEIGTH : NTSC_HEIGTH);
}

void PCMFrame14::writeFrameTo(uint8_t *dest) {
  std::copy(pixels.cbegin(), pixels.cend(), dest);
}

void PCMFrame14::write_data(size_t x, size_t y, std::vector<uint8_t> data) {
  std::copy(data.cbegin(), data.cend(), &pixel_accessor.getLine(y)[x]);
}

std::vector<uint8_t> PCMFrame14::SampletoPixels(uint16_t val) {
  std::vector<uint8_t> res{14};

  for (auto bit = 0; bit < frame_bit_width; ++bit) {
    res[bit] = !!(val & (1 << (sizeof(val) * CHAR_BIT - 1)));
    val <<= 1;
  }

  return res;
}

uint16_t PCMFrame14::SampleFromPixels(const uint8_t *pixels) {
  uint16_t res = 0;
  for (auto bit = 0; bit < frame_bit_width; ++bit) {
    if (pixels[bit]) {
      res |= 1u << (sizeof(res) * CHAR_BIT - 1);
    }
    res >>= 1;
  }
  return res >> 2;
}

PCMFrame14::PCMFrame14(FrameType frameType)
    : pixels(size_t((frameType == PAL) ? WIDTH * PAL_HEIGTH
                                       : WIDTH * NTSC_HEIGTH)),
      frameType{frameType}, pixel_accessor{pixels.data(), WIDTH,
                                           (frameType == PAL) ? PAL_HEIGTH
                                                              : NTSC_HEIGTH} {}

void PCMFrame14::setAudioSample(PCMFrame14::Chanel chanel, uint32_t number,
                                uint16_t sample_data) {
  const size_t sample_line = calc_sample_line(chanel, number);
  const size_t sample_column = calc_sample_column(chanel, number);

  write_data(column2offset(sample_column), sample_line,
             SampletoPixels(sample_data));
}

uint16_t PCMFrame14::getAudioSample(PCMFrame14::Chanel chanel,
                                    uint32_t number) {

  const size_t sample_line = calc_sample_line(chanel, number);
  const size_t sample_column = calc_sample_column(chanel, number);

  auto v = read_data<uint8_t[14]>(column2offset(sample_column), sample_line);

  return SampleFromPixels(v);
}

uint16_t PCMFrame14::generateP(uint32_t startrow) {
  uint16_t res;

  size_t row = startrow;
  size_t column = 0;
  for (; column < pcm_P_column; ++column, row += pcm_shift_rows) {
  }

  return res;
}

uint16_t PCMFrame14::generateQ(uint32_t startrow) {
  uint16_t res;
  size_t row = startrow;
  size_t column = 0;
  for (; column < pcm_P_column; ++column, row += pcm_shift_rows) {
  }
  return res;
}

uint16_t PCMFrame14::generateCRC(uint32_t row) {
  uint16_t res;
  for (size_t column = 0; column < pcm_Q_column; ++column) {
  }
  return res;
}
