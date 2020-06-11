#include <string>
#include <thread>

#include <iostream>

#include "pcmfrmagemanager.h"

PCMFrmageManager::PCMFrmageManager(
    bool isPal, LockingQueue<std::unique_ptr<PCMFrame>> &outQeue)
    : heigth{getHeigth(isPal)}, inputQueue{1}, outQeue{outQeue},
      currentFrame{std::make_unique<PCMFrame>(heigth)},
      nextFrame{std::make_unique<PCMFrame>(heigth)}, mainItherator{
                                                         *currentFrame} {}

PCMFrmageManager::~PCMFrmageManager() {
  if (pThread != nullptr) {
    join();
  }
}

PCMFrmageManager &PCMFrmageManager::start() {
  if (pThread != nullptr) {
    throw std::string("Allready sarted!");
  }

  pThread.reset(new std::thread{
      [](PCMFrmageManager *_this) { _this->thread_func(); }, this});

  return *this;
}

void PCMFrmageManager::join() { pThread->join(); }

size_t PCMFrmageManager::getHeigth(bool isPal) {
  return isPal ? PCMFrmageManager::PAL_HEIGTH : PCMFrmageManager::NTSC_HEIGTH;
}

void PCMFrmageManager::thread_func() {
  PCMLine line;
  while (true) {
    inputQueue.pop(line);

    if (line.isEOF()) {
      process_redy_frame();
      outQeue.push(std::unique_ptr<PCMFrame>());
      break;
    }

    int counter = 0;
    for (auto it = line.iterator(); it != line.pCRC(); ++it) {
      auto &dest = *mainItherator;

      dest = *it;

      if (mainItherator.lastItem()) {
        process_redy_frame();
      }

      ++mainItherator;
      if (!mainItherator.valid()) {
        swapBuffers();
        mainItherator = mainItherator.wrap(*currentFrame);
      }
    }
  }
}

void PCMFrmageManager::process_redy_frame() {
  std::unique_ptr<PCMFrame> processedFrame = std::make_unique<PCMFrame>(heigth);

  std::swap(processedFrame, currentFrame);

  generateCRC(processedFrame);
  outQeue.push(std::move(processedFrame));
}

void PCMFrmageManager::generateCRC(std::unique_ptr<PCMFrame> &frame) {
  for (auto line = 0; line < heigth; ++line) {
    auto pl = frame->getLine(line);
    *pl->pCRC() = pl->generateCRC();
  }
}
