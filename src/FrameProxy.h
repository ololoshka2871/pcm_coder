#ifndef FRAMEPROXY_H
#define FRAMEPROXY_H

#include <memory>

#include "iframe.h"

struct FrameProxy : public IFrame {
  FrameProxy(std::unique_ptr<IFrame> &src_frame, int32_t src_heigth,
             int32_t crop_up, int32_t crop_down, uint8_t factor);

  PixelContainer render(uint8_t grayLevel = default_gray_lvl,
                        uint8_t white_lvl = default_white_lvl) const override;

  bool Eof() const override { return false; }

private:
  std::unique_ptr<IFrame> &src_frame;
  int32_t crop_up, crop_down;
  uint8_t factor;
};

struct FrameProxy2 : public IFrame {
  FrameProxy2(const IFrame &src_frame, int32_t crop_up, int32_t crop_down,
              uint8_t factor);

  PixelContainer render(uint8_t grayLevel = default_gray_lvl,
                        uint8_t white_lvl = default_white_lvl) const override;

  bool Eof() const override { return false; }

private:
  const IFrame &src_frame;
  int32_t crop_up, crop_down;
  uint8_t factor;
};

const std::pair<int32_t, int32_t> &getExtends(bool isPal);

#endif // FRAMEPROXY_H
