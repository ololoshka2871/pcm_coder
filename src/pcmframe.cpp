#include "MyTwoDimArray.h"

#include "pcmframe.h"

PCMFrame::PCMFrame(size_t heigth) : heigth{heigth}, data(heigth * WIDTH) {}

std::vector<uint8_t> PCMFrame::toPixels(uint8_t grayLevel,
                                        uint8_t white_lvl) const {
  std::vector<uint8_t> res(PIXEL_WIDTH * heigth);
  myTwoDimArray<uint8_t> accessor(res.data(), PIXEL_WIDTH, heigth);

  for (auto line = 0; line < heigth; ++line) {
    accessor.element(1, line) = grayLevel;
    accessor.element(3, line) = grayLevel;

    auto pcmline = getLine(line);
    for (auto bitn = 0; bitn < PCMLine::TOTAL_BITS_PRE_LINE; ++bitn) {
      if (pcmline->getBit(bitn)) {
        accessor.element(3 + bitn, line) = grayLevel;
      }
    }

    accessor.element(134, line) = white_lvl;
    accessor.element(135, line) = white_lvl;
    accessor.element(136, line) = white_lvl;
    accessor.element(137, line) = white_lvl;
  }

  return res;
}

PCMLine *PCMFrame::getLine(size_t line_n) {
  return reinterpret_cast<PCMLine *>(&data.at(line_n * WIDTH));
}

const PCMLine *PCMFrame::getLine(size_t line_n) const {
  return reinterpret_cast<const PCMLine *>(&data.at(line_n * WIDTH));
}
