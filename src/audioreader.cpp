
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avassert.h>
#include <libavutil/frame.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include <libavutil/motion_vector.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h>
}

#include "print_duration.h"

#include "FFmpegException.h"

#include "audioreader.h"

struct AudioReader::Context {
  Context(const std::string &filename) : use_resempler{false} {
    int32_t res;

    pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
      throw FFmpegException{AVERROR_BUG};
    }

    res = avformat_open_input(&pFormatContext, filename.c_str(), nullptr,
                              nullptr);
    if (res < 0) {
      throw FFmpegException{res};
    }

    res = avformat_find_stream_info(pFormatContext, nullptr);
    if (res < 0) {
      throw FFmpegException{res};
    }

    for (unsigned int i = 0; i < pFormatContext->nb_streams; i++) {
      auto pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
      auto pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
      if (pLocalCodec == NULL) {
        throw FFmpegException{AVERROR_BUG};
      }

      // when the stream is a video we store its index, codec parameters and
      // codec
      if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
        audio_stream_index = i;
        pCodec = pLocalCodec;
        pCodecParameters = pLocalCodecParameters;
        break;
      }
    }

    pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) {
      throw FFmpegException{AVERROR_BUG};
    }

    res = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (res < 0) {
      throw FFmpegException{res};
    }

    res = avcodec_open2(pCodecContext, pCodec, nullptr);
    if (res < 0) {
      throw FFmpegException{res};
    }

    pFrame = av_frame_alloc();
    if (pFrame == nullptr) {
      throw FFmpegException{ENOMEM};
    }

    pPacket = av_packet_alloc();
    if (pPacket == nullptr) {
      throw FFmpegException{ENOMEM};
    }

    /**********************************/

    AVChannelLayout ch_layout_stereo = AV_CHANNEL_LAYOUT_STEREO;
    AVChannelLayout *pSrcLayout = &pCodecContext->ch_layout;
    if (pCodecContext->ch_layout.nb_channels <= 0) {
        pSrcLayout = &ch_layout_stereo;
    }

    bool is_source_stereo = (av_channel_layout_compare(pSrcLayout, &ch_layout_stereo) == 0);
    if (pCodecContext->sample_fmt != AV_SAMPLE_FMT_S16 ||
        pCodecContext->sample_rate != output_sample_rate ||
        !is_source_stereo) {

      resampler_ctx = nullptr;
      int err = swr_alloc_set_opts2(&resampler_ctx,
                                    &ch_layout_stereo,
                                    AV_SAMPLE_FMT_S16,
                                    output_sample_rate,
                                    pSrcLayout,
                                    pCodecContext->sample_fmt,
                                    pCodecContext->sample_rate,
                                    0,
                                    nullptr);
      if (err < 0 || !resampler_ctx) {
          throw FFmpegException{err ? err : ENOMEM};
      }

      if (resampler_ctx == nullptr) {
        throw FFmpegException{ENOMEM};
      }

      pResampledFrame = av_frame_alloc();
      if (pResampledFrame == nullptr) {
        throw FFmpegException{ENOMEM};
      }
      use_resempler = true;
    }
  }

  ~Context() {
    if (use_resempler) {
      swr_free(&resampler_ctx);
      av_frame_free(&pResampledFrame);
    }
    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
  }

  AVFormatContext *pFormatContext;
  uint32_t audio_stream_index;
  const AVCodec *pCodec;
  AVCodecParameters *pCodecParameters;
  AVCodecContext *pCodecContext;
  AVFrame *pFrame;
  AVPacket *pPacket;

  AVFrame *pResampledFrame;
  SwrContext *resampler_ctx;

  bool use_resempler;
};

AudioReader::AudioReader(const std::string &filename)
    : ctx{std::make_unique<Context>(filename)} {}

AudioReader::~AudioReader() {}

void AudioReader::dumpFileInfo(std::ostream &os) const {
  using namespace std;

  const std::chrono::duration<double> d{ctx->pFormatContext->duration /
                                        (double)AV_TIME_BASE};

  const auto bitrate = ctx->pCodecContext->bit_rate / 1000.0;

  os << "Audio info:" << endl << "\tDuration: " << duration() << endl;
  if (bitrate > 0) {
    os << "\tBitrate: " << bitrate << " Kb/s" << endl;
  }
  os << "\tSample rate: " << ctx->pCodecContext->sample_rate << " Hz" << endl;
}

std::chrono::nanoseconds AudioReader::duration() const {
  const std::chrono::duration<double> d{ctx->pFormatContext->duration /
                                        (double)AV_TIME_BASE};
  return std::chrono::duration_cast<std::chrono::nanoseconds>(d);
}

bool AudioReader::getNextAudioData(const AudioSample_t *&pData,
                                   uint32_t &nb_samples,
                                   std::chrono::nanoseconds &timestamp) {
  while (1) {
    auto res = avcodec_receive_frame(ctx->pCodecContext, ctx->pFrame);
    if (res == AVERROR(EAGAIN)) {
      if (!read_next_audio_pocket()) {
        return false;
      }
      res = avcodec_send_packet(ctx->pCodecContext, ctx->pPacket);
      if (res != 0) {
        throw FFmpegException{res};
      }
      unref_current_pocket();
      continue;
    } else if (res == AVERROR_EOF) {
      return false;
    } else if (res < 0) {
      throw FFmpegException{res};
    }
    break;
  }

  if (ctx->use_resempler) {
    auto pResampledFrame = ctx->pResampledFrame;
    av_frame_unref(pResampledFrame);
    av_channel_layout_from_mask(&pResampledFrame->ch_layout, AV_CH_LAYOUT_STEREO);
    pResampledFrame->sample_rate = output_sample_rate;
    pResampledFrame->format = AV_SAMPLE_FMT_S16;

     if (ctx->pFrame->ch_layout.nb_channels <= 0) {
        av_channel_layout_uninit(&ctx->pFrame->ch_layout);
        av_channel_layout_from_mask(&ctx->pFrame->ch_layout, AV_CH_LAYOUT_STEREO);
    }

    auto err =
        swr_convert_frame(ctx->resampler_ctx, pResampledFrame, ctx->pFrame);
    if (err != 0) {
      throw FFmpegException{err};
    }

    pData =
        reinterpret_cast<AudioSample_t *>(&ctx->pResampledFrame->data[0][0]);
    nb_samples = ctx->pResampledFrame->nb_samples;
  } else {
    pData = reinterpret_cast<AudioSample_t *>(&ctx->pFrame->data[0][0]);
    nb_samples = ctx->pFrame->nb_samples;
  }

  auto tb =
      av_q2d(ctx->pFormatContext->streams[ctx->audio_stream_index]->time_base);

  timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::duration<double>(ctx->pFrame->best_effort_timestamp * tb));

  return true;
}

bool AudioReader::read_next_audio_pocket() {
  while (1) {
    auto res = av_read_frame(ctx->pFormatContext, ctx->pPacket);
    if (res < 0) {
      return false;
    }

    // if it's the audio stream
    if (ctx->pPacket->stream_index == ctx->audio_stream_index) {
      return true;
    }
    unref_current_pocket();
  }
}

void AudioReader::unref_current_pocket() { av_packet_unref(ctx->pPacket); }
