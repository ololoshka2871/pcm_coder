#ifndef LINEDUPLICATORSTAGE_H
#define LINEDUPLICATORSTAGE_H

#include "abstractprocessingstage.h"

#include "iframe.h"

struct PixelDuplicatorStage : public AbstractprocessingStage<IFrame, IFrame> {
  PixelDuplicatorStage(bool PAL = true, int32_t cut_top = 0,
                       int32_t cut_bot = 0, uint8_t factor = 1);

  void Ressive(const IFrame &source) override;

private:
  int32_t cut_top, cut_bot;

  uint8_t factor;

  bool PAL;
};

#endif // LINEDUPLICATORSTAGE_H
