#include "samplestairsitherator.h"

bool SampleStairsItherator::valid() const {
  return (row >= 0) && (row < frame->heigth());
}

bool SampleStairsItherator::lastItem() const {
  return (row == frame->heigth() - 1) && (column == PCMFrame::L0_COLUMN_NUMBER);
}

SampleStairsItherator SampleStairsItherator::wrap(PCMFrame &newframe) {
  return SampleStairsItherator{
      &newframe,
      (row < 0) ? ssize_t(newframe.heigth() + row) // wrap up
                : ssize_t(row - frame->heigth()),  // wrap down
      column};
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
