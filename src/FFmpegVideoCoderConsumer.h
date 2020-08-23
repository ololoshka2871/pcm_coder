#ifndef FFMPEGVIDEOCODERCONSUMER_H
#define FFMPEGVIDEOCODERCONSUMER_H

#include <memory>

#include "IConsumer.h"
#include "iframe.h"

struct FFmpegVideoCoderConsumer : public IConsumer<IFrame> {

  FFmpegVideoCoderConsumer(const std::string &filename,
                           const std::string &codec, const uint32_t bitrate,
                           bool PAL = true);
  ~FFmpegVideoCoderConsumer() override;

  void Ressive(const IFrame &frame) override;

private:
  struct Context;
  std::unique_ptr<Context> ctx;
};

#endif // FFMPEGVIDEOCODERCONSUMER_H
