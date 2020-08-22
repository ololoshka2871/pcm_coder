#include "pcmframe.h"

#include "PCMFrmageStage.h"

static size_t getHeigth(bool isPal) {
  return isPal ? PCMFrame::PAL_HEIGTH : PCMFrame::NTSC_HEIGTH;
}

PCMLine buildHeaderLine(bool copy_protection, bool have_P, bool have_Q) {
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

PCMFrmageStage::PCMFrmageStage(bool is14bit, bool generate_P, bool generate_Q,
                               bool copy_protection, bool isPal)
    : heigth{getHeigth(isPal)}, headerlune{buildHeaderLine(
                                    copy_protection, generate_P, generate_Q)},
      currentFrame{std::make_unique<PCMFrame>(getHeigth(isPal), headerlune)},
      nextFrame{std::make_unique<PCMFrame>(getHeigth(isPal), headerlune)},
      mainItherator{*currentFrame}, is14Bit{is14bit} {}

void PCMFrmageStage::Ressive(const PCMLine &line) {
  if (line.isEOF()) {
    process_redy_frame();

    Send(PCMFrameEof());
    return;
  }

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

void PCMFrmageStage::process_redy_frame() {
  std::unique_ptr<PCMFrame> processedFrame =
      std::make_unique<PCMFrame>(heigth, headerlune);

  std::swap(processedFrame, currentFrame);
  mainItherator = mainItherator.changeFrame(*currentFrame);

  generateCRC(processedFrame);

  Send(*processedFrame);
}

void PCMFrmageStage::generateCRC(std::unique_ptr<PCMFrame> &frame) {
  for (auto line = 0; line < frame->dataHeigth(); ++line) {
    auto pl = frame->getLine(line);

    if (!is14Bit) {
      *pl->pQ() = pl->generate16BitExtention();
      pl->shiftMainDataAndP();
    }

    *pl->pCRC() = pl->generateCRC();
  }
}
