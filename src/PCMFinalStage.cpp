#include <thread>

#include "PCMFinalStage.h"

PCMFinalStage::PCMFinalStage(size_t width, size_t heigth, uint32_t queueSize)
    : inputQueue{queueSize}, width{width}, heigth{heigth} {}

PCMFinalStage::~PCMFinalStage() {
  if (pThread != nullptr) {
    join();
  }
}

void PCMFinalStage::setPolicy(
    std::unique_ptr<IPCMFrameProcessingPolicy> &&policy) {
  processingPolicy = std::move(policy);

  if (processingPolicy != nullptr) {
    processingPolicy->Init(width, heigth);
  }
}

PCMFinalStage &PCMFinalStage::start() {
  if (pThread != nullptr) {
    throw std::string("Allready sarted!");
  }

  pThread.reset(new std::thread{
      [](PCMFinalStage *_this) { _this->thread_func(); }, this});

  return *this;
}

void PCMFinalStage::join() { pThread->join(); }

void PCMFinalStage::thread_func() {
  std::unique_ptr<IFrame> frame;
  while (true) {
    inputQueue.pop(std::move(frame));

    if (frame == nullptr) {
      break;
    }

    if (processingPolicy != nullptr) {
      (*processingPolicy)(frame);
    }
  }
  if (processingPolicy != nullptr) {
    processingPolicy->finish();
  }
}
