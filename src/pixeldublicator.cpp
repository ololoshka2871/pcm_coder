#include <cassert>
#include <cstring>

#include "MyTwoDimArray.h"

#include "frame.h"

#include "pixeldublicator.h"

struct FrameProxy : public IFrame {
  FrameProxy(std::unique_ptr<IFrame> &src_frame, uint8_t factor)
      : IFrame(src_frame->width() * factor, src_frame->heigth()),
        src_frame{src_frame}, factor{factor} {}

  PixelContainer render(uint8_t grayLevel = default_gray_lvl,
                        uint8_t white_lvl = default_white_lvl) const override {
    auto src_pixels = src_frame->render(grayLevel, white_lvl);

    auto wide_frame = std::make_unique<Frame>(src_pixels.width() * factor,
                                              src_pixels.heigth());

    auto src_accessor = src_pixels.getAccessor();
    myTwoDimArray<uint8_t> dest_accessor{
        wide_frame->data_pixels(), static_cast<int32_t>(wide_frame->width()),
        static_cast<int32_t>(wide_frame->heigth())};

    for (auto line = 0; line < src_accessor.getheigth(); ++line) {
      auto src = src_accessor.getLine(line);
      auto dst = dest_accessor.getLine(line);
      for (auto src_pixel_n = 0; src_pixel_n < src_accessor.getwidth();
           ++src_pixel_n) {
        std::memset(dst, *src, factor);
        dst += factor;
        ++src;
      }
    }

    return wide_frame->render(grayLevel, white_lvl);
  }

private:
  std::unique_ptr<IFrame> &src_frame;
  uint8_t factor;
};

PixelDublicator::PixelDublicator(uint8_t factor) : factor{factor} {
  assert(factor > 0);
}

void PixelDublicator::Init(size_t width, size_t heigth) {
  consumer->Init(width * factor, heigth);
}

void PixelDublicator::operator()(std::unique_ptr<IFrame> &frame) {
  if (factor == 1) {
    (*consumer)(frame);
    return;
  } else {
    std::unique_ptr<IFrame> proxy{new FrameProxy(frame, factor)};
    (*consumer)(proxy);
  }
}

void PixelDublicator::finish() { consumer->finish(); }
