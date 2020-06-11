#include "ffmpegvideocoder.h"

FFmpegVideoCoder::FFmpegVideoCoder(size_t width, size_t heigth,
                                   const std::string &filename,
                                   const std::string &codec,
                                   const uint32_t bitrate, bool cut,
                                   uint32_t queueSize)
    : AbastractPCMFinalStage(width, heigth, queueSize) {}

FFmpegVideoCoder::~FFmpegVideoCoder() {}

void FFmpegVideoCoder::processPCMFrame(std::unique_ptr<PCMFrame> &frame) {}
