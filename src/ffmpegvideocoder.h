#ifndef FFMPEGVIDEOCODER_H
#define FFMPEGVIDEOCODER_H

#include <memory>

#include "ipcmframeprocessingpolicy.h"

struct FfmpegWriteContext;

struct FFmpegVideoCoder : public IPCMFrameProcessingPolicy {
  static constexpr auto PIXEL_WIDTH = 720;

  FFmpegVideoCoder(const std::string &filename, const std::string &codec,
                   const uint32_t bitrate = -1, bool PAL = true);
  ~FFmpegVideoCoder();

  void Init(size_t width, size_t heigth) override;
  void operator()(std::unique_ptr<IFrame> &frame) override;
  void finish() override;

private:
  std::unique_ptr<FfmpegWriteContext> encoder;

  size_t width, heigth;

  std::string filename;
  std::string codec;
  uint32_t bitrate;
  bool PAL;
};

#endif // FFMPEGVIDEOCODER_H
