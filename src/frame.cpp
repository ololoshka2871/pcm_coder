#include <cstring>

#include "MyTwoDimArray.h"

#include "frame.h"

Frame::~Frame() {}

IFrame::PixelContainer Frame::render(uint8_t grayLevel,
                                     uint8_t white_lvl) const {

  /*
const auto pixel_heigth = SYNC_LINE_2 + 1 + width() + 1 + WHITE_WIDTH + 1;

std::vector<uint8_t> res(pixel_heigth * heigth());

myTwoDimArray<uint8_t> dest(res.data(), pixel_heigth, heigth());
myTwoDimArray<uint8_t> src((void *)m_pixels.data(),
                           static_cast<int32_t>(width()),
                           static_cast<int32_t>(heigth()));

for (auto l = 0; l < heigth(); ++l) {
  auto line = dest.getLine(l);

  line[SYNC_LINE_1] = grayLevel;
  line[SYNC_LINE_2] = grayLevel;

        std::memcpy(&line[5], src.getLine(l), heigth());

        std::memset(&line[WHITE_LINE], white_lvl, WHITE_WIDTH);
}
*/

  return m_pixels;
}
