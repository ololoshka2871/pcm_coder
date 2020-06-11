#ifndef FFMPEGVIDEOCODER_H
#define FFMPEGVIDEOCODER_H

#include "AbastractPCMFinalStage.h"

struct FFmpegVideoCoder : public AbastractPCMFinalStage {
  FFmpegVideoCoder(size_t width, size_t heigth, const std::string &filename,
                   const std::string &codec, const uint32_t bitrate = -1,
                   bool cut = false);

  ~FFmpegVideoCoder() override;

protected:
  void processPCMFrame(std::unique_ptr<PCMFrame> &frame) override;
};

#endif // FFMPEGVIDEOCODER_H
