#ifndef SPLITTER_H
#define SPLITTER_H

#include <algorithm>
#include <list>

#include "abstractprocessingstage.h"

template <typename U> struct Splitter : public AbstractprocessingStage<U, U> {
  template <typename T>
  AbstractprocessingStage<U, T> &
  AddStage(AbstractprocessingStage<U, T> *newconsumer) {
    brances.emplace_back(newconsumer);
    return *newconsumer;
  }

  IConsumer<U> &AddConsumer(IConsumer<U> *newconsumer) {
    brances.emplace_back(newconsumer);
    return *newconsumer;
  }

  void Ressive(const U &data) override { Send(data); }

protected:
  void Send(const U &product) override {
    for (auto it = brances.cbegin(); it != brances.cend(); ++it) {
      (*it)->Ressive(product);
    }
    AbstractprocessingStage<U, U>::Send(product);
  }

private:
  std::list<std::shared_ptr<IConsumer<U>>> brances;
};

#endif // SPLITTER_H
