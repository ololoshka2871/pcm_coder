#include <thread>

#include <SDL2/SDL.h>

#include "sdl2display.h"

SDL2Display::SDL2Display(size_t width, size_t heigth,
                         const std::function<void()> &onClose)
    : AbastractPCMFinalStage(width, heigth), quit{false} {
  SDL_Init(SDL_INIT_VIDEO);

  window =
      SDL_CreateWindow("PCM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       width, heigth, SDL_WINDOW_VULKAN);

  pThread.reset(new std::thread{[this, onClose]() {
    SDL_Event event;
    while (!quit) {
      SDL_WaitEvent(&event);

      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        onClose();
        break;
      }
    }
  }});
}

SDL2Display::~SDL2Display() {
  quit = true;
  pThread->join();
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void SDL2Display::processPCMFrame(const std::unique_ptr<PCMFrame> &frame) {}
