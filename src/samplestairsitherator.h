#ifndef SAMPLESTAIRSITHERATOR_H
#define SAMPLESTAIRSITHERATOR_H

#include <cstddef>
#include <iterator>

#include "pcmframe.h"

struct SampleStairsItherator {
  // Итератор по PCMFrame "лесенкой"

  static constexpr auto stairs_offset = 16;
  static constexpr auto TotalStairsVerticalSize =
      stairs_offset * PCMFrame::DATA_COLUMNS;

  typedef std::forward_iterator_tag iterator_category;
  typedef int difference_type;

  SampleStairsItherator(PCMFrame &frame) : frame{&frame}, row{0}, column{0} {}

  bool valid() const;
  bool lastItem() const;

  SampleStairsItherator wrap(PCMFrame &newframe);
  SampleStairsItherator changeFrame(PCMFrame &newframe);

  uint16_t &operator*();
  SampleStairsItherator &operator++();

private:
  SampleStairsItherator(PCMFrame *frame, int32_t row, size_t column)
      : frame{frame}, row{row}, column{column} {}

  PCMFrame *frame;
  int32_t row;
  size_t column;
};

#endif // SAMPLESTAIRSITHERATOR_H
