#ifndef SDL2DISPLAY_H
#define SDL2DISPLAY_H

#include <functional>
#include <memory>

#include "ipcmframeprocessingpolicy.h"

namespace std {
class thread;
}

struct SDL2Display : public IPCMFrameProcessingPolicy {
  SDL2Display(const std::function<void()> &onClose, uint32_t queueSize = 1);

  ~SDL2Display();

  void Init(size_t width, size_t heigth) override;
  void operator()(std::unique_ptr<IFrame> &frame) override;

private:
  struct Context;

  std::unique_ptr<Context> ctx;

  void GuiThread();
  void renderFrame();
};

#endif // SDL2DISPLAY_H
