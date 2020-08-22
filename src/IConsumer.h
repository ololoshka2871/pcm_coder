#ifndef ICONSUMER_H
#define ICONSUMER_H

template <typename T> struct IConsumer {

  virtual ~IConsumer() = default;

  virtual void Ressive(const T &) = 0;
};

#endif // ICONSUMER_H
