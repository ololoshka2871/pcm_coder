#ifndef PCMFRAME_H
#define PCMFRAME_H

#include <climits>

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

  static constexpr auto PAL_HEIGTH = 588;
  static constexpr auto NTSC_HEIGTH = 490;

  static constexpr auto HEADER_SIZE_LINES = 2;

  PCMFrame(size_t heigth, const PCMLine &headerLine);

  PixelContainer render(uint8_t grayLevel = 150,
                        uint8_t white_lvl = 255) const override;

  PCMLine *getLine(size_t line_n);
  const PCMLine *getLine(size_t line_n) const;

  size_t heigth() const override;

private:
  std::vector<PCMLine> data;
  const PCMLine &headerLine;
};

#endif // PCMFRAME_H
