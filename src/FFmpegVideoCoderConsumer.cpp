#include "FFmpegVideoCoderConsumer.h"

FFmpegVideoCoderConsumer::FFmpegVideoCoderConsumer(const std::string &filename,
                                                   const std::string &codec,
                                                   const uint32_t bitrate,
                                                   bool PAL)
    : encoder{filename, codec, bitrate, PAL}, started{false} {}

void FFmpegVideoCoderConsumer::Ressive(const IFrame &frame) {
  if (!started) {
    encoder.Init(frame.width(), frame.heigth());
    started = true;
  }

  if (started && frame.Eof()) {
    encoder.finish();
  } else {
    encoder(frame);
  }
}
