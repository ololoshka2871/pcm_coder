#include "ffmpegvideocoder.h"

FFmpegVideoCoder::FFmpegVideoCoder(const std::string &filename,
                                   const std::string &codec,
                                   const uint32_t bitrate, bool cut,
                                   uint32_t queueSize) {}

void FFmpegVideoCoder::Init(size_t width, size_t heigth) {
  this->width = width;
  this->heigth = heigth;
}

void FFmpegVideoCoder::operator()(std::unique_ptr<IFrame> &frame) {}
