#ifndef PCMFRAME_H
#define PCMFRAME_H

#include <climits>
#include <cstdint>

#include "iframe.h"

#include "pcmline.h"

struct PCMFrame : public IFrame {
  friend struct SampleStairsItherator;

  // промежуточный кадр. Содержит в себе PCM-данные как uint16_t
  // [L R L R L R P Q CRC]

  static constexpr auto WIDTH = PCMLine::TotalDataPreLineWithCRC;

  static constexpr auto PIXEL_WIDTH = 139;

  static constexpr auto L0_COLUMN_NUMBER = 0;
  static constexpr auto DATA_COLUMNS = PCMLine::Q_offset + 1;

  static constexpr auto CRC1_COLUMN_NUMBER = DATA_COLUMNS;
  static constexpr auto TOTAL_LINE_ELEMENTS = CRC1_COLUMN_NUMBER + 1;

  static constexpr auto PAL_HEIGTH = 590;
  static constexpr auto NTSC_HEIGTH = 492;

  static constexpr auto HEADER_SIZE_LINES = 2;

  PCMFrame(size_t heigth, const PCMLine &headerLine);

  PixelContainer render(uint8_t grayLevel = 150,
                        uint8_t white_lvl = 255) const override;

  PCMLine *getLine(size_t line_n);
  const PCMLine *getLine(size_t line_n) const;

  int32_t dataHeigth() const override;

  bool Eof() const override { return false; }

private:
  std::vector<PCMLine> data;
  const PCMLine &headerLine;
};

struct PCMFrameEof : public IFrame {
  PCMFrameEof() : IFrame{0, 0} {}

  PixelContainer render(uint8_t grayLevel = default_gray_lvl,
                        uint8_t white_lvl = default_white_lvl) const override {
    return PixelContainer(1, 1);
  }

  bool Eof() const override { return true; }
};

#endif // PCMFRAME_H
