#ifndef FRAME_H
#define FRAME_H

#include "iframe.h"

struct Frame : public IFrame {
  Frame(size_t width, size_t heigth)
      : IFrame(width, heigth), m_pixels(width, heigth) {}

  ~Frame();

  PixelContainer render(uint8_t grayLevel = default_gray_lvl,
                        uint8_t white_lvl = default_white_lvl) const override;

  uint8_t *data_pixels() { return m_pixels.pixels.data(); }

private:
  PixelContainer m_pixels;
};

#endif // FRAME_H
