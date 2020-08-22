#ifndef RPIFBDISPLAYCONSUMER_H
#define RPIFBDISPLAYCONSUMER_H

#include "IConsumer.h"

#include "iframe.h"

struct RPIFbDisplayConsumer : public IConsumer<IFrame> {
  RPIFbDisplayConsumer();

  void Ressive(const IFrame &frame) override;
};

#endif // RPIFBDISPLAYCONSUMER_H
