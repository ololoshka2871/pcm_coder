#include <iostream>

#include "SDL2DisplayConsumer.h"

void SDL2DisplayConsumer::InitRenderer(int width, int heigth) {
  this->width = width;
  this->heigth = heigth;

  window =
      SDL_CreateWindow("PCM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width, heigth, SDL_WINDOW_ALLOW_HIGHDPI);

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == nullptr) {
    std::cerr << "Failed to create SDL renderer: " << SDL_GetError()
              << std::endl;
    throw 0;
  }
}

void SDL2DisplayConsumer::renderFrame(const IFrame &frame) {
#if 1
  auto pixels = frame.render();
  void *p = pixels.pixels.data();

  auto surface = SDL_CreateRGBSurfaceFrom(p, frame.width(), frame.heigth(), 8,
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
  if (texture == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface: " << SDL_GetError()
              << std::endl;
    return;
  }

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
