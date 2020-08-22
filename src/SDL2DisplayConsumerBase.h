#ifndef PIXELDUPLICATORSTAGEBASE_H
#define PIXELDUPLICATORSTAGEBASE_H

#include <functional>
#include <map>
#include <mutex>

#include <SDL2/SDL.h>

#include "IConsumer.h"

#include "iframe.h"

using rgb565 = uint16_t;

struct SDL2DisplayConsumerBase : public IConsumer<IFrame> {
  SDL2DisplayConsumerBase();

  static void VideoInit();

  ~SDL2DisplayConsumerBase() override;

  void Ressive(const IFrame &frame) override;

  void onClose(const std::function<void()> &cb);

  virtual void InitRenderer(int width, int heigth) = 0;

  static std::tuple<int, int> getDisplaySize();
  static bool DetectPALNTSC();

private:
  std::function<void()> exit_cb;

  std::map<uint8_t, rgb565> pixelcache;

  static void buildGrayscalePalete();

protected:
  SDL_Window *window;
  SDL_Renderer *renderer;
  int32_t width, heigth;

  static SDL_Palette *palete;

  virtual void renderFrame(const IFrame &frame) = 0;
};

#endif // PIXELDUPLICATORSTAGEBASE_H
