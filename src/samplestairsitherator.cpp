#include "samplestairsitherator.h"

bool SampleStairsItherator::valid() const {
  return (row >= 0) && (row < frame->dataHeigth());
}

bool SampleStairsItherator::lastItem() const {
  return (row == frame->dataHeigth() - 1) &&
         (column == PCMFrame::L0_COLUMN_NUMBER);
}

SampleStairsItherator SampleStairsItherator::wrap(PCMFrame &newframe) {
  return SampleStairsItherator{
      &newframe,
      (row < 0) ? int32_t(newframe.dataHeigth() + row) // wrap up
                : int32_t(row - frame->dataHeigth()),  // wrap down
      column};
}

SampleStairsItherator SampleStairsItherator::changeFrame(PCMFrame &newframe) {
  return SampleStairsItherator{&newframe, row, column};
}

uint16_t &SampleStairsItherator::operator*() {
  return (*frame->getLine(row))[column];
}

SampleStairsItherator &SampleStairsItherator::operator++() {
  static constexpr auto wrap_back_rows =
      stairs_offset * (PCMFrame::DATA_COLUMNS - 1);

  ++column;
  if (column == PCMFrame::CRC1_COLUMN_NUMBER) {
    column = PCMFrame::L0_COLUMN_NUMBER;
    row -= wrap_back_rows - 1;
  } else {
    row += stairs_offset;
  }

  return *this;
}
