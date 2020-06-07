#ifndef SDL2DISPLAY_H
#define SDL2DISPLAY_H

#include <functional>
#include <memory>

#include "AbastractPCMFinalStage.h"

namespace std {
class thread;
}

struct SDL_Window;

struct SDL2Display : public AbastractPCMFinalStage {
  SDL2Display(size_t width, size_t heigth,
              const std::function<void()> &onClose);

  ~SDL2Display() override;

protected:
  void processPCMFrame(const std::unique_ptr<PCMFrame> &frame) override;

private:
  SDL_Window *window;

  std::unique_ptr<std::thread> pThread;

  bool quit;
};

#endif // SDL2DISPLAY_H
