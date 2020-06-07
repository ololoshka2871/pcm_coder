#include "ffmpegvideocoder.h"

FFmpegVideoCoder::FFmpegVideoCoder(size_t width, size_t heigth,
                                   const std::string &filename,
                                   const std::string &codec,
                                   const uint32_t bitrate, bool cut)
    : AbastractPCMFinalStage(width, heigth) {}

FFmpegVideoCoder::~FFmpegVideoCoder() {}

void FFmpegVideoCoder::processPCMFrame(const std::unique_ptr<PCMFrame> &frame) {

}
