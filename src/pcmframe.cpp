#include <cstring>

#include "MyTwoDimArray.h"

#include "pcmframe.h"

PCMFrame::PCMFrame(size_t heigth) : IFrame(PIXEL_WIDTH, heigth), data(heigth) {}

std::vector<uint8_t> PCMFrame::toPixels(uint8_t grayLevel,
                                        uint8_t white_lvl) const {
  std::vector<uint8_t> res(width() * heigth());
  myTwoDimArray<uint8_t> dest(res.data(), width(), heigth());

  for (auto l = 0; l < heigth(); ++l) {
    auto line = dest.getLine(l);

    line[SYNC_LINE_1] = grayLevel;
    line[SYNC_LINE_2] = grayLevel;

    auto pcmline = getLine(l);
    for (auto bitn = 0; bitn < PCMLine::TOTAL_DATA_BITS_PRE_LINE; ++bitn) {
      if (pcmline->getBit(bitn)) {
        line[DATA_OFFSET_PIXELS + bitn] = grayLevel;
      }
    }

    {
      auto crc_but_n = 0;
      for (auto bitn = PCMLine::TOTAL_DATA_BITS_PRE_LINE;
           bitn < PCMLine::TOTAL_BITS_PRE_LINE; ++bitn, ++crc_but_n) {
        if (pcmline->getCRCBit(crc_but_n)) {
          line[DATA_OFFSET_PIXELS + bitn] = grayLevel;
        }
      }
    }

    std::memset(&line[WHITE_LINE], white_lvl, WHITE_WIDTH);
  }

  return res;
}

PCMLine *PCMFrame::getLine(size_t line_n) { return &data.at(line_n); }

const PCMLine *PCMFrame::getLine(size_t line_n) const {
  return &data.at(line_n);
}
