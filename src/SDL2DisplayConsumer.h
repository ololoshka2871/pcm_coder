#ifndef SDL2DISPLAYCONSUMER_H
#define SDL2DISPLAYCONSUMER_H

#include "SDL2DisplayConsumerBase.h"

struct SDL2DisplayConsumer : public SDL2DisplayConsumerBase {
  void InitRenderer(int width, int heigth) override;

protected:
  void renderFrame(const IFrame &frame) override;
};

#endif // SDL2DISPLAYCONSUMER_H
