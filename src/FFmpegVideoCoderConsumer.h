#ifndef FFMPEGVIDEOCODERCONSUMER_H
#define FFMPEGVIDEOCODERCONSUMER_H

#include "IConsumer.h"

#include "iframe.h"

struct FFmpegVideoCoderConsumer : public IConsumer<IFrame> {
  FFmpegVideoCoderConsumer();

  void Ressive(const IFrame &frame) override;
};

#endif // FFMPEGVIDEOCODERCONSUMER_H
