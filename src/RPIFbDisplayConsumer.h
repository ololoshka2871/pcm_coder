#ifndef RPIFBDISPLAYCONSUMER_H
#define RPIFBDISPLAYCONSUMER_H

#include "SDL2DisplayConsumerBase.h"

struct RPIFbDisplayConsumer : public SDL2DisplayConsumerBase {
  void InitRenderer(int width, int heigth) override;

protected:
  void renderFrame(const IFrame &frame) override;
};

#endif // RPIFBDISPLAYCONSUMER_H
