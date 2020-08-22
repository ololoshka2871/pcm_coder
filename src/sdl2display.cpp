#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include <SDL2/SDL.h>

#include "sdl2display.h"

using rgb565 = uint16_t;

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

struct SDL2Display::Context {
  Context(const std::function<void()> &onClose)
      : onClose{onClose}, quit{false} {}

  std::function<void()> onClose;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  std::unique_ptr<SDL_Event> event;

  std::unique_ptr<std::thread> pThread;

  std::mutex frame_guard;
  std::vector<uint8_t> frameToDisplay;

  std::map<uint8_t, rgb565> pixelcache;

  size_t width, heigth;

  bool quit;
};

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

SDL2Display::SDL2Display(const std::function<void()> &onClose,
                         uint32_t queueSize)
    : ctx{std::make_unique<Context>(onClose)} {}

SDL2Display::~SDL2Display() {
  /*if (!ctx->quit) {*/
  if (ctx->event) {
    ctx->event->type = SDL_QUIT;
    SDL_PushEvent(ctx->event.get());
  }
  ctx->quit = true;
  ctx->pThread->join();
  //}
}

void SDL2Display::Init(size_t width, size_t heigth) {
  ctx->width = width;
  ctx->heigth = heigth;

  ctx->pThread.reset(
      new std::thread{[](SDL2Display *_this) { _this->GuiThread(); }, this});
}

void SDL2Display::operator()(std::unique_ptr<IFrame> &frame) {
  {
    std::lock_guard guard(ctx->frame_guard);
    ctx->frameToDisplay = std::move(frame->render().pixels);
  }

  if (ctx->event) {
    ctx->event->type = SDL_USEREVENT;
    SDL_PushEvent(ctx->event.get());
  }
}

void SDL2Display::GuiThread() {
  SDL_Init(SDL_INIT_VIDEO);

  ctx->window =
      SDL_CreateWindow("PCM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       ctx->width, ctx->heigth, SDL_WINDOW_ALLOW_HIGHDPI);

  ctx->renderer = SDL_CreateRenderer(
      ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (ctx->renderer == nullptr) {
    std::cerr << "Failed to create SDL renderer: " << SDL_GetError()
              << std::endl;
    throw 0;
  }

  {
    auto fmt = SDL_PIXELFORMAT_RGB565;
    ctx->texture =
        SDL_CreateTexture(ctx->renderer, fmt, SDL_TEXTUREACCESS_STREAMING,
                          ctx->width, ctx->heigth);
    {
      uint32_t f;
      SDL_QueryTexture(ctx->texture, &f, nullptr, nullptr, nullptr);
      assert(f == fmt);
    }
  }

  reset_texture(ctx->texture, ctx->width, ctx->heigth);

  ctx->event = std::make_unique<SDL_Event>();

  while (!ctx->quit) {
    SDL_WaitEvent(ctx->event.get());

    switch (ctx->event->type) {
    case SDL_USEREVENT:
      renderFrame();
      break;
    case SDL_QUIT:
      ctx->quit = true;
      ctx->onClose();
      break;
    }
  }

  SDL_DestroyTexture(ctx->texture);
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
  SDL_Quit();
}

void SDL2Display::renderFrame() {
#if 1
  uint8_t *pixels;
  int pitch;

  auto &cache = ctx->pixelcache;

  auto res = SDL_LockTexture(ctx->texture, nullptr, (void **)&pixels, &pitch);
  if (res == 0) {
    {
      std::lock_guard guard(ctx->frame_guard);

      auto graysacle_data = &ctx->frameToDisplay[0];

      for (auto line = 0; line < ctx->heigth; ++line) {
        auto base = reinterpret_cast<rgb565 *>(&pixels[line * pitch]);
        for (auto p = 0; p < ctx->width; ++p) {
          auto pixel = *graysacle_data++;
          try {
            base[p] = cache.at(pixel);
          } catch (std::out_of_range) {
            cache[pixel] = Initrgb565gray(pixel);
            base[p] = cache.at(pixel);
          }
        }
      }
    }
    SDL_UnlockTexture(ctx->texture);
  } else {
    std::cerr << "Locking texture failed: " << SDL_GetError() << std::endl;
  }

  SDL_RenderClear(ctx->renderer);
  res = SDL_RenderCopy(ctx->renderer, ctx->texture, nullptr, nullptr);
  if (res != 0) {
    std::cerr << "SDL_RenderCopy: " << res << std::endl;
    return;
  }
  SDL_RenderPresent(ctx->renderer);
#endif
}
