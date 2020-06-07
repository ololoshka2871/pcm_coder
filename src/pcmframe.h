#ifndef PCMFRAME_H
#define PCMFRAME_H

#include <climits>

#include "pcmline.h"

struct PCMFrame {
  friend struct SampleStairsItherator;

  // промежуточный кадр. Содержит в себе PCM-данные как uint16_t
  // [L R L R L R P Q CRC0 CRC1]

  static constexpr auto WIDTH =
      ((PCMLine::TotalDataPreLine) + sizeof(uint16_t));

  static constexpr auto L0_COLUMN_NUMBER = 0;
  static constexpr auto DATA_COLUMNS = PCMLine::Q_offset + 1;

  static constexpr auto CRC1_COLUMN_NUMBER = DATA_COLUMNS;
  static constexpr auto CRC2_COLUMN_NUMBER = CRC1_COLUMN_NUMBER + 1;
  static constexpr auto TOTAL_LINE_ELEMENTS = CRC2_COLUMN_NUMBER + 1;

  PCMFrame(size_t heigth);

private:
  size_t heigth;

  std::vector<uint16_t> data;
};

#endif // PCMFRAME_H
