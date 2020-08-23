#include "ffmpegvideocoder.h"

#include "FFmpegVideoCoderConsumer.h"

struct FFmpegVideoCoderConsumer::Context {
  Context(const std::string &filename, const std::string &codec,
          const uint32_t bitrate, bool PAL)
      : encoder{filename, codec, bitrate, PAL}, started{false} {}

  FFmpegVideoCoder encoder;
  bool started;
};

FFmpegVideoCoderConsumer::FFmpegVideoCoderConsumer(const std::string &filename,
                                                   const std::string &codec,
                                                   const uint32_t bitrate,
                                                   bool PAL)
    : ctx{std::make_unique<Context>(filename, codec, bitrate, PAL)} {}

FFmpegVideoCoderConsumer::~FFmpegVideoCoderConsumer() {}

void FFmpegVideoCoderConsumer::Ressive(const IFrame &frame) {
  if (!ctx->started) {
    ctx->encoder.Init(frame.width(), frame.heigth());
    ctx->started = true;
  }

  if (ctx->started && frame.Eof()) {
    ctx->encoder.finish();
  } else {
    ctx->encoder(frame);
  }
}
