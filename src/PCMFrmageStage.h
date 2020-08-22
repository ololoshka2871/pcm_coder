#ifndef PCMFRMAGESTAGE_H
#define PCMFRMAGESTAGE_H

#include "abstractprocessingstage.h"

#include "iframe.h"
#include "pcmline.h"

#include "samplestairsitherator.h"

struct PCMFrmageStage : public AbstractprocessingStage<PCMLine, IFrame> {
  PCMFrmageStage(bool is14bit, bool generate_P, bool generate_Q,
                 bool copy_protection, bool isPal);

  void Ressive(const PCMLine &line) override;

private:
  size_t heigth;
  PCMLine headerlune;
  std::unique_ptr<PCMFrame> currentFrame;
  std::unique_ptr<PCMFrame> nextFrame;
  SampleStairsItherator mainItherator;
  bool is14Bit;

  void process_redy_frame();
  void generateCRC(std::unique_ptr<PCMFrame> &frame);

  void swapBuffers() { std::swap(currentFrame, nextFrame); }
};

#endif // PCMFRMAGESTAGE_H
