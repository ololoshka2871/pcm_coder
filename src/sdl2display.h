#ifndef SDL2DISPLAY_H
#define SDL2DISPLAY_H

#include <functional>
#include <memory>

#include "AbastractPCMFinalStage.h"

namespace std {
class thread;
}

struct SDL2Display : public AbastractPCMFinalStage {
  SDL2Display(size_t width, size_t heigth, const std::function<void()> &onClose,
              uint32_t queueSize = 1);

  ~SDL2Display() override;

protected:
  void processPCMFrame(std::unique_ptr<PCMFrame> &frame) override;

private:
  struct Context;

  std::unique_ptr<Context> ctx;

  void GuiThread();
  void renderFrame();
};

#endif // SDL2DISPLAY_H
