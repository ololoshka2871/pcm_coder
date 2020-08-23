#ifndef FFMPEGVIDEOCODERCONSUMER_H
#define FFMPEGVIDEOCODERCONSUMER_H

#include <memory>
#include <string>

#include "IConsumer.h"

#include "iframe.h"

#include "ffmpegvideocoder.h"

struct FfmpegWriteContext;

struct FFmpegVideoCoderConsumer : public IConsumer<IFrame> {
  FFmpegVideoCoderConsumer(const std::string &filename,
                           const std::string &codec, const uint32_t bitrate,
                           bool PAL = true);

  void Ressive(const IFrame &frame) override;

private:
  FFmpegVideoCoder encoder;
  bool started;
};

#endif // FFMPEGVIDEOCODERCONSUMER_H
