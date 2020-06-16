#include <cstring>

#include "pcmframe.h"

PCMFrame::PCMFrame(size_t heigth, const PCMLine &headerLine)
    : IFrame(PIXEL_WIDTH, heigth),
      data(heigth - HEADER_SIZE_LINES), headerLine{headerLine} {}

IFrame::PixelContainer PCMFrame::render(uint8_t grayLevel,
                                        uint8_t white_lvl) const {
  PixelContainer res{width(), IFrame::heigth()};
  auto dest = res.getAccessor();

  // header
  for (auto l = 0; l < HEADER_SIZE_LINES; ++l) {
    auto line = dest.getLine(l);
    line[SYNC_LINE_1] = grayLevel;
    line[SYNC_LINE_2] = grayLevel;

    headerLine.renderTo(&line[DATA_OFFSET_PIXELS], grayLevel);

    std::memset(&line[WHITE_LINE], white_lvl, WHITE_WIDTH);
  }

  // 0 -> 0
  // 1 -> 2
  // 2 -> 4
  // 3 -> 6

  // height / 2 - 1 -> 1
  // height / 2 - 0 -> 3

  size_t height = heigth();
  for (size_t dl = 0; dl < height; ++dl) {
    size_t l; // deinterlace destination
    if (dl < height / 2) {
      l = dl * 2;
    } else {
      l = (dl - height / 2) * 2 + 1;
    }

    auto line = dest.getLine(l + HEADER_SIZE_LINES);

    line[SYNC_LINE_1] = grayLevel;
    line[SYNC_LINE_2] = grayLevel;

    auto pcmline = getLine(dl);

    pcmline->renderTo(&line[DATA_OFFSET_PIXELS], grayLevel);

    std::memset(&line[WHITE_LINE], white_lvl, WHITE_WIDTH);
  }

  return res;
}

PCMLine *PCMFrame::getLine(size_t line_n) { return &data.at(line_n); }

const PCMLine *PCMFrame::getLine(size_t line_n) const {
  return &data.at(line_n);
}

int32_t PCMFrame::heigth() const {
  return IFrame::heigth() - HEADER_SIZE_LINES;
}
