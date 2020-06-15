#include <string>
#include <thread>

#include <iostream>

#include "pcmfrmagemanager.h"

PCMFrmageManager::PCMFrmageManager(
    bool generate_P, bool generate_Q, bool copy_protection, bool isPal,
    LockingQueue<std::unique_ptr<IFrame>> &outQeue, uint32_t quieueSize)
    : heigth{getHeigth(isPal)}, headerlune{buildHeaderLine(
                                    copy_protection, generate_P, generate_Q)},
      inputQueue{quieueSize}, outQeue{outQeue},
      currentFrame{std::make_unique<PCMFrame>(heigth, headerlune)},
      nextFrame{std::make_unique<PCMFrame>(heigth, headerlune)},
      mainItherator{*currentFrame} {}

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
  return isPal ? PCMFrame::PAL_HEIGTH : PCMFrame::NTSC_HEIGTH;
}

void PCMFrmageManager::thread_func() {
  PCMLine line;
  int frame_counter = 0;

  while (true) {
    inputQueue.pop(line);

    if (line.isEOF()) {
      process_redy_frame();
      outQeue.push(std::unique_ptr<PCMFrame>());
      break;
    }

    for (auto it = line.iterator(); it != line.pCRC(); ++it) {
      auto &dest = *mainItherator;

      dest = *it;

      if (mainItherator.lastItem()) {
        process_redy_frame();
#if 0
        std::cout << "Generated " << frame_counter++ << " frames" << std::endl;
#endif
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
  std::unique_ptr<PCMFrame> processedFrame =
      std::make_unique<PCMFrame>(heigth, headerlune);

  std::swap(processedFrame, currentFrame);

  generateCRC(processedFrame);
  outQeue.push(std::move(processedFrame));
}

void PCMFrmageManager::generateCRC(std::unique_ptr<PCMFrame> &frame) {
  for (auto line = 0; line < frame->heigth(); ++line) {
    auto pl = frame->getLine(line);
    *pl->pCRC() = pl->generateCRC();
  }
}

PCMLine PCMFrmageManager::buildHeaderLine(bool copy_protection, bool have_P,
                                          bool have_Q) {
  PCMLine res;

  res[0] = 0x3333;
  res[1] = 0xCCCC;
  res[2] = 0x3333;
  res[3] = 0xCCCC;

  uint16_t _copy_protection = copy_protection;
  uint16_t _have_P = !have_P;
  uint16_t _have_Q = !have_Q;

  res[7] = (1 << 0) |               // Pre-emphasis [false inverted]
           (_have_Q << 1) |         // Q [inverted]
           (_have_P << 2) |         // P [inverted]
           (_copy_protection << 3); // Copy-protect

  *res.pCRC() = res.generateCRC();

  return res;
}
