#include <iostream>

#include "RPIFbDisplayConsumer.h"

void RPIFbDisplayConsumer::InitRenderer(int width, int heigth) {
  this->width = width;
  this->heigth = heigth;

  window =
      SDL_CreateWindow("PCM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, heigth, SDL_WINDOW_BORDERLESS);

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == nullptr) {
    std::cerr << "Failed to create SDL renderer: " << SDL_GetError()
              << std::endl;
    throw 0;
  }

  // TODO init vsync
}

void RPIFbDisplayConsumer::renderFrame(const IFrame &frame) {
#if 1
  void *pixels = frame.render().pixels.data();

  auto surface = SDL_CreateRGBSurfaceFrom(pixels, frame.width(), heigth, 8,
                                          frame.width(), 0, 0, 0, 0);

  if (surface == nullptr) {
    std::cerr << "SDL_CreateRGBSurfaceFrom: " << SDL_GetError() << std::endl;
    return;
  }

  auto r = SDL_SetSurfacePalette(surface, palete);
  if (r == -1) {
    std::cerr << "SDL_SetPalette: " << SDL_GetError() << std::endl;
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  SDL_RenderClear(renderer);
  auto res = SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  if (res != 0) {
    std::cerr << "SDL_RenderCopy: " << res << std::endl;
    return;
  }
  SDL_RenderPresent(renderer);

  SDL_DestroyTexture(texture);
#endif
}
