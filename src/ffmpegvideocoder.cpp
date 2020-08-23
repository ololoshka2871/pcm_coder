#include <cstring>

#include "frame.h"

#include "WriteContext.h"

#include "ffmpegvideocoder.h"

FFmpegVideoCoder::FFmpegVideoCoder(const std::string &filename,
                                   const std::string &codec,
                                   const uint32_t bitrate, bool PAL)
    : filename{filename}, codec{codec}, bitrate{bitrate}, PAL{PAL} {}

FFmpegVideoCoder::~FFmpegVideoCoder() {}

void FFmpegVideoCoder::Init(size_t width, size_t heigth) {
  this->width = width;
  this->heigth = heigth;

  encoder = std::make_unique<FfmpegWriteContext>(
      filename, codec, this->heigth, PIXEL_WIDTH, bitrate, PAL, width);
}

void FFmpegVideoCoder::operator()(std::unique_ptr<IFrame> &frame) {
  if (encoder != nullptr) {
    auto pixels = frame->render();
    encoder->encode_frame(pixels.pixels.data());
  }
}

void FFmpegVideoCoder::operator()(const IFrame &frame) {
  auto pixels = frame.render();
  encoder->encode_frame(pixels.pixels.data());
}

void FFmpegVideoCoder::finish() { encoder.reset(); }
