#ifndef ABSTRACTPROCESSINGSTAGE_H
#define ABSTRACTPROCESSINGSTAGE_H

#include <memory>

#include "IConsumer.h"

template <typename Ti, typename To>
struct AbstractprocessingStage : public IConsumer<Ti> {
  IConsumer<To> &NextConsumer(IConsumer<To> *newconsumer) {
    conumer.reset(newconsumer);
    return *newconsumer;
  }

  template <typename T>
  AbstractprocessingStage<To, T> &
  NextStage(AbstractprocessingStage<To, T> *newconsumer) {
    conumer.reset(newconsumer);
    return *newconsumer;
  }

protected:
  virtual void Send(const To &product) { conumer->Ressive(product); }

  std::shared_ptr<IConsumer<To>> conumer;
};

#endif // ABSTRACTPROCESSINGSTAGE_H
