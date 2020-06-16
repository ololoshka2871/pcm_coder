#ifndef PIXELDUBLICATOR_H
#define PIXELDUBLICATOR_H

#include "ipcmframeprocessingpolicy.h"

struct FrameExtender : public IPCMFrameProcessingPolicy {
  FrameExtender(bool PAL = true, int32_t cut_top = 0, int32_t cut_bot = 0,
                uint8_t factor = 1);

  void Init(size_t width, size_t heigth) override;
  void operator()(std::unique_ptr<IFrame> &frame) override;
  void finish() override;

  void setConsumer(std::unique_ptr<IPCMFrameProcessingPolicy> &&rr) {
    consumer = std::move(rr);
  }

private:
  std::unique_ptr<IPCMFrameProcessingPolicy> consumer;

  int32_t cut_top, cut_bot;

  int32_t src_heigth;

  uint8_t factor;

  bool PAL;
};

#endif // PIXELDUBLICATOR_H
