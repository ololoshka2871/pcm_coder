#ifndef PIXELDUBLICATOR_H
#define PIXELDUBLICATOR_H

#include "ipcmframeprocessingpolicy.h"

struct PixelDublicator : public IPCMFrameProcessingPolicy {
  PixelDublicator(uint8_t factor = 1);

  void Init(size_t width, size_t heigth) override;
  void operator()(std::unique_ptr<IFrame> &frame) override;
  void finish() override;

  void setConsumer(std::unique_ptr<IPCMFrameProcessingPolicy> &&rr) {
    consumer = std::move(rr);
  }

private:
  std::unique_ptr<IPCMFrameProcessingPolicy> consumer;

  uint8_t factor;
};

#endif // PIXELDUBLICATOR_H
