#include <cstring>

#include "frame.h"

#include "FrameProxy.h"

FrameProxy::FrameProxy(std::unique_ptr<IFrame> &src_frame, int32_t src_heigth,
                       int32_t crop_up, int32_t crop_down, uint8_t factor)
    : IFrame(src_frame->width() * factor, src_heigth - crop_up - crop_down),
      src_frame{src_frame}, crop_up{crop_up}, crop_down{crop_down},
      factor{factor} {}

IFrame::PixelContainer FrameProxy::render(uint8_t grayLevel,
                                          uint8_t white_lvl) const {
  auto src_pixels = src_frame->render(grayLevel, white_lvl);

  auto wide_frame =
      std::make_unique<Frame>(src_pixels.width() * factor, heigth());

  auto src_accessor = src_pixels.getAccessor();
  myTwoDimArray<uint8_t> dest_accessor{
      wide_frame->data_pixels(), static_cast<int32_t>(wide_frame->width()),
      static_cast<int32_t>(wide_frame->heigth())};

  auto dest_line = 0;
  auto src_line = 0;
  auto lines_to_copy = src_pixels.heigth();

  if (crop_up < 0) {
    // extend black lines on top count crop_up
    dest_line = -crop_up;
  } else {
    src_line = crop_up;
    lines_to_copy -= crop_up;
  }

  if (crop_down > 0) {
    lines_to_copy -= crop_down;
  }

  if (lines_to_copy > 0) {
    for (auto l = 0; l < lines_to_copy; ++l, ++src_line, ++dest_line) {
      auto src = src_accessor.getLine(src_line);
      auto dst = dest_accessor.getLine(dest_line);
      for (auto src_pixel_n = 0; src_pixel_n < src_accessor.getwidth();
           ++src_pixel_n) {
        if (*src) {
          std::memset(dst, *src, factor);
        }
        dst += factor;
        ++src;
      }
    }
  }

  return wide_frame->render(grayLevel, white_lvl);
}

FrameProxy2::FrameProxy2(const IFrame &src_frame, int32_t crop_up,
                         int32_t crop_down, uint8_t factor)
    : IFrame(src_frame.width() * factor,
             src_frame.heigth() - crop_up - crop_down),
      src_frame{src_frame}, crop_up{crop_up}, crop_down{crop_down},
      factor{factor} {}

IFrame::PixelContainer FrameProxy2::render(uint8_t grayLevel,
                                           uint8_t white_lvl) const {

  auto src_pixels = src_frame.render(grayLevel, white_lvl);

  auto wide_frame =
      std::make_unique<Frame>(src_pixels.width() * factor, heigth());

  auto src_accessor = src_pixels.getAccessor();
  myTwoDimArray<uint8_t> dest_accessor{
      wide_frame->data_pixels(), static_cast<int32_t>(wide_frame->width()),
      static_cast<int32_t>(wide_frame->heigth())};

  auto dest_line = 0;
  auto src_line = 0;
  auto lines_to_copy = src_pixels.heigth();

  if (crop_up < 0) {
    // extend black lines on top count crop_up
    dest_line = -crop_up;
  } else {
    src_line = crop_up;
    lines_to_copy -= crop_up;
  }

  if (crop_down > 0) {
    lines_to_copy -= crop_down;
  }

  if (lines_to_copy > 0) {
    for (auto l = 0; l < lines_to_copy; ++l, ++src_line, ++dest_line) {
      auto src = src_accessor.getLine(src_line);
      auto dst = dest_accessor.getLine(dest_line);
      for (auto src_pixel_n = 0; src_pixel_n < src_accessor.getwidth();
           ++src_pixel_n) {
        if (*src) {
          std::memset(dst, *src, factor);
        }
        dst += factor;
        ++src;
      }
    }
  }

  return wide_frame->render(grayLevel, white_lvl);
}

const std::pair<int32_t, int32_t> &getExtends(bool isPal) {
  // extemd frame up and down
  static const std::pair<int32_t, int32_t> expends[]{
      {18, 15}, // NTSC
      {14, 21}, // PAL
  };

  return expends[isPal];
}
