#ifndef SDL2DISPLAYCONSUMER_H
#define SDL2DISPLAYCONSUMER_H

#include <functional>
#include <map>
#include <mutex>

#include <SDL2/SDL.h>

#include "IConsumer.h"

#include "iframe.h"

using rgb565 = uint16_t;

struct SDL2DisplayConsumer : public IConsumer<IFrame> {
  SDL2DisplayConsumer();

  ~SDL2DisplayConsumer() override;

  void Ressive(const IFrame &frame) override;

  void onClose(const std::function<void()> &cb);

private:
  std::function<void()> exit_cb;

  SDL_Window *window;
  SDL_Renderer *renderer;
  // SDL_Texture *texture;

  std::map<uint8_t, rgb565> pixelcache;

  int32_t width, heigth;

  void renderFrame(const std::vector<uint8_t> &frameToDisplay);
  void writeTexture(const std::vector<uint8_t> &frameToDisplay, int pitch,
                    uint8_t *pixels);
};

#endif // SDL2DISPLAYCONSUMER_H
