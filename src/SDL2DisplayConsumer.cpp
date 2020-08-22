#include <climits>

#include <cstring>
#include <iostream>

#include "SDL2DisplayConsumer.h"

static SDL_Palette *palete;

static void buildGrayscalePalete() {
  static constexpr int palete_size = 1 << CHAR_BIT;

  palete = SDL_AllocPalette(palete_size);
  for (int i = 0; i < palete_size; i++) {
    palete->colors[i].r = palete->colors[i].g = palete->colors[i].b = i;
  }
}

rgb565 Initrgb565() { return 0; }

rgb565 Initrgb565gray(uint8_t light) {
  uint32_t r = (float)light * 31.0f / 255.0f;
  r &= 0x1F;
  uint32_t g = (float)light * 63.0f / 255.0f;
  g &= 0x3F;
  uint32_t b = (float)light * 31.0f / 255.0f;
  b &= 0x1F;
  return (r << (5 + 6)) | (g << 5) | (b);
}

static void reset_texture(SDL_Texture *texture, int width, int heigth) {

  rgb565 black = Initrgb565();

  rgb565 *pixels;
  int pitch;

  auto res = SDL_LockTexture(texture, nullptr, (void **)&pixels, &pitch);
  if (res == 0) {
    for (auto line = 0; line < heigth; ++line) {
      for (auto p = 0; p < pitch / sizeof(rgb565); ++p) {
        pixels[line * pitch / sizeof(rgb565) + p] = black;
      }
    }
    SDL_UnlockTexture(texture);
  } else {
    std::cerr << "Locking texture failed: " << SDL_GetError() << std::endl;
  }
}

SDL2DisplayConsumer::SDL2DisplayConsumer() : window{} {
  SDL_Init(SDL_INIT_VIDEO);

  buildGrayscalePalete();
}

SDL2DisplayConsumer::~SDL2DisplayConsumer() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

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

void SDL2DisplayConsumer::Ressive(const IFrame &frame) {
  if (window == nullptr) {
    InitRenderer(frame.width(), frame.heigth());
  }

  renderFrame(frame.render().pixels);

  SDL_Event ev;
  SDL_WaitEventTimeout(&ev, 0);

  if (ev.type == SDL_QUIT) {
    if (exit_cb) {
      exit_cb();
    }
  }
}

void SDL2DisplayConsumer::onClose(const std::function<void()> &cb) {
  exit_cb = cb;
}

void SDL2DisplayConsumer::renderFrame(
    const std::vector<uint8_t> &frameToDisplay) {

#if 1
  auto surface = SDL_CreateRGBSurfaceFrom((void *)frameToDisplay.data(), width,
                                          heigth, 8, width, 0, 0, 0, 0);

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
