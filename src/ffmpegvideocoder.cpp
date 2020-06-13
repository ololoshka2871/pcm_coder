#include "WriteContext.h"

#include "ffmpegvideocoder.h"

FFmpegVideoCoder::FFmpegVideoCoder(const std::string &filename,
                                   const std::string &codec,
                                   const uint32_t bitrate, bool PAL, bool cut)
    : filename{filename}, codec{codec}, bitrate{bitrate}, PAL{PAL}, cut{cut} {}

FFmpegVideoCoder::~FFmpegVideoCoder() {}

void FFmpegVideoCoder::Init(size_t width, size_t heigth) {
  this->width = width;
  this->heigth = heigth;

  encoder = std::make_unique<FfmpegWriteContext>(
      filename, codec, heigth, PIXEL_WIDTH, bitrate, PAL, width);
}

void FFmpegVideoCoder::operator()(std::unique_ptr<IFrame> &frame) {
  if (encoder != nullptr) {
    encoder->encode_frame(frame->render().pixels.data());
  }
}

void FFmpegVideoCoder::finish() { encoder.reset(); }
