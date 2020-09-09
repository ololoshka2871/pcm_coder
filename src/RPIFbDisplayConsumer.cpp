#include <condition_variable>
#include <iostream>
#include <mutex>

#include "bcm_host.h"

#include "RPIFbDisplayConsumer.h"

struct RPIFbDisplayConsumer::Context {
  Context() {}

  DISPMANX_DISPLAY_HANDLE_T display;
  std::mutex mutex;
  std::condition_variable cv;
  bool vsync_div = false;
};

void RPIFbDisplayConsumer::InitRenderer(int width, int heigth) {
  this->width = width;
  this->heigth = heigth;

  window =
      SDL_CreateWindow("PCM", 0, 0, 0, 0, SDL_WINDOW_BORDERLESS|SDL_WINDOW_FULLSCREEN);

  SDL_ShowCursor(SDL_DISABLE);

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == nullptr) {
    std::cerr << "Failed to create SDL renderer: " << SDL_GetError()
              << std::endl;
    throw 0;
  }

  ctx->display = vc_dispmanx_display_open(0);

  // BUG: Clear any existing callbacks, even to other apps.
  // https://github.com/raspberrypi/userland/issues/218
  // TODO: Check if we still need this.
  vc_dispmanx_vsync_callback(ctx->display, NULL, NULL);

  // Set the callback function.
  vc_dispmanx_vsync_callback(ctx->display, vsync_callback, this);
}

RPIFbDisplayConsumer::RPIFbDisplayConsumer()
    : ctx{std::make_unique<Context>()} {}

RPIFbDisplayConsumer::~RPIFbDisplayConsumer() {
  vc_dispmanx_vsync_callback(ctx->display, NULL, NULL);
}

void RPIFbDisplayConsumer::renderFrame(const IFrame &frame) {
  auto pixels = frame.render();
  void *p = pixels.pixels.data();

  auto surface = SDL_CreateRGBSurfaceFrom(p, frame.width(), heigth, 8,
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

  {
    SDL_Rect dest{12, 0, width - 10, heigth};
    std::unique_lock<std::mutex> lk(ctx->mutex);
    ctx->cv.wait(lk);

    SDL_RenderClear(renderer);
    auto res = SDL_RenderCopy(renderer, texture, nullptr, &dest);
    if (res != 0) {
      std::cerr << "SDL_RenderCopy: " << res << std::endl;
      return;
    }
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);
  }
}

void RPIFbDisplayConsumer::vsync_callback(DISPMANX_UPDATE_HANDLE_T u,
                                          void *anon_render_shared) {
  auto _this = static_cast<RPIFbDisplayConsumer *>(anon_render_shared);
  if (_this->ctx->vsync_div ^= true) {
    usleep(2000);
    _this->ctx->cv.notify_one();
  }
}
