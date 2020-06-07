#ifndef PCMENCODER_H
#define PCMENCODER_H

#include <string>

#include "pcmframe.h"

#include "LockingQueue.h"

namespace std {
class thread;
}

struct AbastractPCMFinalStage {
  // ждет из очереди PCMFrame
  // Создает новый AVFrame и кодирует PCMFrame в него. затем отправляет FFmpeg'у
  // на запись

  AbastractPCMFinalStage(size_t width, size_t heigth);

  virtual ~AbastractPCMFinalStage();

  AbastractPCMFinalStage &start();
  void join();

  LockingQueue<std::unique_ptr<PCMFrame>> &getQueue() { return inputQueue; }

protected:
  virtual void processPCMFrame(const std::unique_ptr<PCMFrame> &frame) = 0;
  virtual void onThreadExit() {}

  const size_t width;
  const size_t heigth;

private:
  LockingQueue<std::unique_ptr<PCMFrame>> inputQueue;

  std::unique_ptr<std::thread> pThread;

  void thread_func();
};

#endif // PCMENCODER_H
