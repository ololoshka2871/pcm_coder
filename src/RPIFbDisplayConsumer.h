#ifndef RPIFBDISPLAYCONSUMER_H
#define RPIFBDISPLAYCONSUMER_H

#include <memory>

#include "SDL2DisplayConsumerBase.h"

using DISPMANX_UPDATE_HANDLE_T = uint32_t;

struct RPIFbDisplayConsumer : public SDL2DisplayConsumerBase {
  RPIFbDisplayConsumer();
  ~RPIFbDisplayConsumer();

  void InitRenderer(int width, int heigth) override;

protected:
  void renderFrame(const IFrame &frame) override;

private:
  struct Context;
  std::unique_ptr<Context> ctx;

  static void vsync_callback(DISPMANX_UPDATE_HANDLE_T u,
                             void *anon_render_shared);
};

#endif // RPIFBDISPLAYCONSUMER_H
