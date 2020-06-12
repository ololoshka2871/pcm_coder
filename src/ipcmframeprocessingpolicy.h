#ifndef IPCMFRAMEPROCESSINGPOLICY_H
#define IPCMFRAMEPROCESSINGPOLICY_H

#include <memory>

#include "iframe.h"

struct IPCMFrameProcessingPolicy {
  virtual ~IPCMFrameProcessingPolicy() = default;

  virtual void Init(size_t width, size_t heigth) = 0;
  virtual void operator()(std::unique_ptr<IFrame> &frame) = 0;
  virtual void finish() {}
};

#endif // IPCMFRAMEPROCESSINGPOLICY_H
