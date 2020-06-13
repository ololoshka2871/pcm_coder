#ifndef PCMFRMAGEMANAGER_H
#define PCMFRMAGEMANAGER_H

#include <memory>

#include "pcmframe.h"
#include "pcmline.h"
#include "samplestairsitherator.h"

#include "LockingQueue.h"

namespace std {
class thread;
}

struct PCMFrmageManager {
  // хранит 2 PCMFrame
  // забирает из очереди PCMLine
  // записывает данные в PCMFrame
  // когда кадр полностью построен кладет его в очередь на энкодинг

  PCMFrmageManager(bool generate_P, bool generate_Q, bool copy_protection,
                   bool isPal, LockingQueue<std::unique_ptr<IFrame>> &outQeue,
                   uint32_t quieueSize = 1);
  ~PCMFrmageManager();

  PCMFrmageManager &start();
  void join();

  LockingQueue<PCMLine> &getInputQueue() { return inputQueue; }

  static size_t getHeigth(bool isPal);

private:
  size_t heigth;
  PCMLine headerlune;
  LockingQueue<PCMLine> inputQueue;
  LockingQueue<std::unique_ptr<IFrame>> &outQeue;

  std::unique_ptr<std::thread> pThread;

  std::unique_ptr<PCMFrame> currentFrame;
  std::unique_ptr<PCMFrame> nextFrame;

  SampleStairsItherator mainItherator;

  void thread_func();

  void swapBuffers() { std::swap(currentFrame, nextFrame); }

  void process_redy_frame();

  void generateCRC(std::unique_ptr<PCMFrame> &frame);

  static PCMLine buildHeaderLine(uint16_t copy_protection, uint16_t have_P,
                                 uint16_t have_Q);
};

#endif // PCMFRMAGEMANAGER_H
