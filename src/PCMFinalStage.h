#ifndef PCMENCODER_H
#define PCMENCODER_H

#include <string>

#include "pcmframe.h"

#include "ipcmframeprocessingpolicy.h"

#include "LockingQueue.h"

namespace std {
class thread;
}

struct PCMFinalStage {
  // ждет из очереди PCMFrame
  // Создает новый AVFrame и кодирует PCMFrame в него. затем отправляет FFmpeg'у
  // на запись

  PCMFinalStage(size_t width, size_t heigth, uint32_t queuesSize = 1);
  ~PCMFinalStage();

  void setPolicy(std::unique_ptr<IPCMFrameProcessingPolicy> &&policy);

  PCMFinalStage &start();
  void join();

  LockingQueue<std::unique_ptr<IFrame>> &getQueue() { return inputQueue; }

private:
  LockingQueue<std::unique_ptr<IFrame>> inputQueue;

  std::unique_ptr<std::thread> pThread;

  std::unique_ptr<IPCMFrameProcessingPolicy> processingPolicy;

  size_t width, heigth;

  void thread_func();
};

#endif // PCMENCODER_H
