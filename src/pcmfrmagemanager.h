#ifndef PCMFRMAGEMANAGER_H
#define PCMFRMAGEMANAGER_H

#include "pcmframe.h"
#include "pcmline.h"

#include "LockingQueue.h"

struct PCMFrmageManager {
  // хранит 2 PCMFrame
  // забирает из очереди PCMLine
  // записывает данные в PCMFrame
  // когда кадр полностью построен кладет его в очередь на энкодинг

  PCMFrmageManager(LockingQueue<PCMFrame> &ouqQeue) : inpitQueue{1} {}

  PCMFrmageManager &start();
  PCMFrmageManager &finalise();
  void join();

  LockingQueue<PCMLine> &getInputQueue() { return inpitQueue; }

private:
  LockingQueue<PCMLine> inpitQueue;
};

#endif // PCMFRMAGEMANAGER_H
