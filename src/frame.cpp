#include <cstring>

#include "MyTwoDimArray.h"

#include "frame.h"

Frame::~Frame() {}

IFrame::PixelContainer Frame::render(uint8_t grayLevel,
                                     uint8_t white_lvl) const {
  return m_pixels;
}
