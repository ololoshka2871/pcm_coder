#include <climits>

#include <iostream>

#include "SDL2DisplayConsumerBase.h"

SDL_Palette *SDL2DisplayConsumerBase::palete = nullptr;

void SDL2DisplayConsumerBase::buildGrayscalePalete() {
  if (palete == nullptr) {
    static constexpr int palete_size = 1 << CHAR_BIT;

    palete = SDL_AllocPalette(palete_size);
    for (int i = 0; i < palete_size; i++) {
      palete->colors[i].r = palete->colors[i].g = palete->colors[i].b = i;
    }
  }
}

SDL2DisplayConsumerBase::SDL2DisplayConsumerBase() : window{} {
  buildGrayscalePalete();
}

void SDL2DisplayConsumerBase::VideoInit() { SDL_Init(SDL_INIT_VIDEO); }

SDL2DisplayConsumerBase::~SDL2DisplayConsumerBase() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

std::tuple<int, int> SDL2DisplayConsumerBase::getDisplaySize() {
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  return std::make_tuple(DM.w, DM.h);
}

bool SDL2DisplayConsumerBase::DetectPALNTSC() {
  auto [w, h] = getDisplaySize();
  if (w == 720) {
    if (h == 576) {
      return true;
    } else if (h == 480) {
      return false;
    }
  }
  throw h;
}

void SDL2DisplayConsumerBase::Ressive(const IFrame &frame) {
  if (window == nullptr || frame.Eof()) {
    return;
  }

  renderFrame(frame);

  SDL_Event ev;
  SDL_WaitEventTimeout(&ev, 0);

  if (ev.type == SDL_QUIT) {
    if (exit_cb) {
      exit_cb();
    }
  }
}

void SDL2DisplayConsumerBase::onClose(const std::function<void()> &cb) {
  exit_cb = cb;
}
