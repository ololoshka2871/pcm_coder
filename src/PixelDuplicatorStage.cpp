#include "FrameProxy.h"

#include "pcmframe.h"

#include "PixelDuplicatorStage.h"

PixelDuplicatorStage::PixelDuplicatorStage(bool PAL, int32_t cut_top,
                                           int32_t cut_bot, uint8_t factor)
    : cut_top{cut_top}, cut_bot{cut_bot}, factor{factor}, PAL{PAL} {
  assert(factor > 0);
}

void PixelDuplicatorStage::Ressive(const IFrame &source) {
  if (source.Eof()) {
    Send(source);
    return;
  }

  auto &extends = getExtends(PAL);

  int32_t crop_up = cut_top - extends.first;
  int32_t crop_down = cut_bot - extends.second;

  FrameProxy2 proxy{source, crop_up, crop_down, factor};

  Send(proxy);
}

int PixelDuplicatorStage::FrameHeigth(bool PAL, int32_t cut_top,
                                      int32_t cut_bot) {
  auto &extends = getExtends(PAL);

  int32_t crop_up = cut_top - extends.first;
  int32_t crop_down = cut_bot - extends.second;

  return (PAL ? PCMFrame::PAL_HEIGTH : PCMFrame::NTSC_HEIGTH) - crop_up -
         crop_down;
}
