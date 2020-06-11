#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include <SDL2/SDL.h>

#include "sdl2display.h"

union rgb565 {
  struct __attribute__((packed)) {
    unsigned r : 5;
    unsigned g : 6;
    unsigned b : 5;
  };
  uint16_t raw;

  rgb565() : raw(0) {}
  rgb565(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

  rgb565 &operator=(const rgb565 &lr) {
    raw = lr.raw;
    return *this;
  }

  static rgb565 gray(uint8_t light) {
    return rgb565((float)light * 31.0f / 255.0f, (float)light * 63.0f / 255.0f,
                  (float)light * 31.0f / 255.0f);
  }
};

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

  bool quit;
};

static void reset_texture(SDL_Texture *texture, int width, int heigth) {

  rgb565 test = rgb565::gray(0);

  rgb565 *pixels;
  int pitch;

  auto res = SDL_LockTexture(texture, nullptr, (void **)&pixels, &pitch);
  if (res == 0) {
    for (auto line = 0; line < heigth; ++line) {
      for (auto p = 0; p < pitch / sizeof(rgb565); ++p) {
        pixels[line * pitch / sizeof(rgb565) + p] = test;
      }
    }
    SDL_UnlockTexture(texture);
  } else {
    std::cerr << "Locking texture failed: " << SDL_GetError() << std::endl;
  }
}

SDL2Display::SDL2Display(size_t width, size_t heigth,
                         const std::function<void()> &onClose)
    : AbastractPCMFinalStage(width, heigth), ctx{std::make_unique<Context>(
                                                 onClose)} {

  ctx->pThread.reset(
      new std::thread{[](SDL2Display *_this) { _this->GuiThread(); }, this});
}

SDL2Display::~SDL2Display() {
  if (!ctx->quit) {
    ctx->event->type = SDL_QUIT;
    SDL_PushEvent(ctx->event.get());
    ctx->quit = true;
    ctx->pThread->join();
  }
}

void SDL2Display::processPCMFrame(std::unique_ptr<PCMFrame> &frame) {
  {
    std::lock_guard guard(ctx->frame_guard);
    ctx->frameToDisplay = frame->toPixels();
  }

  if (ctx->event) {
    ctx->event->type = SDL_USEREVENT;
    SDL_PushEvent(ctx->event.get());
  }
}

void SDL2Display::GuiThread() {
  SDL_Init(SDL_INIT_VIDEO);

  ctx->window = SDL_CreateWindow(
      "PCM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, heigth,
      SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);

  ctx->renderer = SDL_CreateRenderer(
      ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (ctx->renderer == nullptr) {
    std::cerr << "Failed to create SDL renderer: " << SDL_GetError()
              << std::endl;
    throw 0;
  }

  {
    auto fmt = SDL_PIXELFORMAT_RGB565;
    ctx->texture = SDL_CreateTexture(
        ctx->renderer, fmt, SDL_TEXTUREACCESS_STREAMING, width, heigth);
    {
      uint32_t f;
      SDL_QueryTexture(ctx->texture, &f, nullptr, nullptr, nullptr);
      assert(f == fmt);
    }
  }

  reset_texture(ctx->texture, width, heigth);

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

  std::cerr << "Exit display" << std::endl;

  SDL_DestroyTexture(ctx->texture);
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
  SDL_Quit();
}

void SDL2Display::renderFrame() {
  uint8_t *pixels;
  int pitch;

  auto res = SDL_LockTexture(ctx->texture, nullptr, (void **)&pixels, &pitch);
  if (res == 0) {
    {
      std::lock_guard guard(ctx->frame_guard);
      auto graysacle_data = &ctx->frameToDisplay[0];

      for (auto line = 0; line < heigth; ++line) {
        auto base = reinterpret_cast<rgb565 *>(&pixels[line * pitch]);
        for (auto p = 0; p < width; ++p) {
          auto pixel = *graysacle_data++;
          try {
            base[p] = ctx->pixelcache.at(pixel);
          } catch (std::out_of_range) {
            ctx->pixelcache[pixel] = rgb565::gray(pixel);
            base[p] = ctx->pixelcache.at(pixel);
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
}
