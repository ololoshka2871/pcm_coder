#include <cassert>
#include <cstring>

#include "MyTwoDimArray.h"

#include "frame.h"

#include "FrameProxy.h"

#include "frameextender.h"

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
