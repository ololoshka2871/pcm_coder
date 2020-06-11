#include <thread>

#include "AbastractPCMFinalStage.h"

AbastractPCMFinalStage::AbastractPCMFinalStage(size_t width, size_t heigth,
                                               uint32_t queueSize)
    : inputQueue{queueSize}, width{width}, heigth{heigth} {}

AbastractPCMFinalStage::~AbastractPCMFinalStage() {
  if (pThread != nullptr) {
    join();
  }
}

AbastractPCMFinalStage &AbastractPCMFinalStage::start() {
  if (pThread != nullptr) {
    throw std::string("Allready sarted!");
  }

  pThread.reset(new std::thread{
      [](AbastractPCMFinalStage *_this) { _this->thread_func(); }, this});

  return *this;
}

void AbastractPCMFinalStage::join() { pThread->join(); }

void AbastractPCMFinalStage::thread_func() {
  std::unique_ptr<PCMFrame> frame;
  while (true) {
    inputQueue.pop(std::move(frame));

    if (frame == nullptr) {
      break;
    }

    processPCMFrame(frame);
  }
  onThreadExit();
}
