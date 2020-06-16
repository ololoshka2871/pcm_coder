#include <cassert>
#include <cstring>

#include "MyTwoDimArray.h"

#include "frame.h"

#include "frameextender.h"

static const std::pair<int32_t, int32_t> &getExtends(bool isPal) {
  // extemd frame up and down
  static const std::pair<int32_t, int32_t> expends[]{
      {18, 15}, // NTSC
      {14, 21}, // PAL
  };

  return expends[isPal];
}

/*
    auto container = frame->render();
    auto src_accessor = container.getAccessor();

    IFrame::PixelContainer ppFrame{width, heigth};
    auto dest_accessor = ppFrame.getAccessor();

    auto expend = getExtends(PAL);

    int32_t first_src_line = std::max(crop_up - expend.first, 0);
    uint32_t last_src_line = width;

    uint32_t dest_line = std::max(expend.first - crop_up, 0);
    auto line = first_src_line;
    for (; line < 200; ++line, ++dest_line) {
      auto _dest = dest_accessor.getLine(dest_line);
      auto _src = src_accessor.getLine(line);
      auto _w = frame->width();
      std::memcpy(_dest, _src, _w);
    }
 */

struct FrameProxy : public IFrame {
  FrameProxy(std::unique_ptr<IFrame> &src_frame, int32_t src_heigth,
             int32_t crop_up, int32_t crop_down, uint8_t factor)
      : IFrame(src_frame->width() * factor, src_heigth - crop_up - crop_down),
        src_frame{src_frame}, crop_up{crop_up}, crop_down{crop_down},
        factor{factor} {}

  PixelContainer render(uint8_t grayLevel = default_gray_lvl,
                        uint8_t white_lvl = default_white_lvl) const override {
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
          std::memset(dst, *src, factor);
          dst += factor;
          ++src;
        }
      }
    }

    return wide_frame->render(grayLevel, white_lvl);
  }

private:
  std::unique_ptr<IFrame> &src_frame;
  int32_t crop_up, crop_down;
  uint8_t factor;
};

FrameExtender::FrameExtender(bool PAL, int32_t cut_top, int32_t cut_bot,
                             uint8_t factor)
    : cut_top{cut_top}, cut_bot{cut_bot}, factor{factor},
      src_heigth{}, PAL{PAL} {
  assert(factor > 0);
}

void FrameExtender::Init(size_t width, size_t heigth) {
  auto &extends = getExtends(PAL);

  src_heigth = heigth;

  auto result_heigth =
      heigth + extends.first + extends.second - cut_top - cut_bot;

  consumer->Init(width * factor, result_heigth);
}

void FrameExtender::operator()(std::unique_ptr<IFrame> &frame) {
  auto &extends = getExtends(PAL);

  int32_t crop_up = cut_top - extends.first;
  int32_t crop_down = cut_bot - extends.second;

  std::unique_ptr<IFrame> proxy{
      new FrameProxy(frame, src_heigth, crop_up, crop_down, factor)};
  (*consumer)(proxy);
}

void FrameExtender::finish() { consumer->finish(); }
