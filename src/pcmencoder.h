#ifndef PCMENCODER_H
#define PCMENCODER_H

#include <string>

#include "pcmframe.h"

#include "LockingQueue.h"

struct PCMEncoder {
  // ждет из очереди PCMFrame
  // Создает новый AVFrame и кодирует PCMFrame в него. затем отправляет FFmpeg'у
  // на запись

  PCMEncoder(const std::string &filename, const std::string &codec,
             const uint32_t bitrate = -1, bool cut = false)
      : inputQueue{1} {}

  PCMEncoder &start();
  PCMEncoder &finalise();
  void join();

  LockingQueue<PCMFrame> &getQueue() { return inputQueue; }

private:
  LockingQueue<PCMFrame> inputQueue;
};

#endif // PCMENCODER_H
