#ifndef FFMPEGVIDEOCODER_H
#define FFMPEGVIDEOCODER_H

#include "ipcmframeprocessingpolicy.h"

struct FFmpegVideoCoder : public IPCMFrameProcessingPolicy {
  FFmpegVideoCoder(const std::string &filename, const std::string &codec,
                   const uint32_t bitrate = -1, bool cut = false,
                   uint32_t queueSize = 1);

  void Init(size_t width, size_t heigth) override;
  void operator()(std::unique_ptr<IFrame> &frame) override;

private:
  size_t width, heigth;
};

#endif // FFMPEGVIDEOCODER_H
