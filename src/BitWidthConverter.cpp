#include "samplegenerator.h"

#include "BitWidthConverter.h"

struct BitWidthConverter::Context {
  Context(bool is_14_bit, bool use_dither) : generator{is_14_bit, use_dither} {}

  SampleGenerator generator;
};

BitWidthConverter::BitWidthConverter(bool is_14_bit, bool use_dither)
    : ctx{std::make_unique<Context>(is_14_bit, use_dither)} {}

void BitWidthConverter::Ressive(
    const AudioProdusser::AudioPacket &audiopacket) {
  if (audiopacket.eof()) {
    Send(SamplesPackEof{});
    return;
  }
  Send(SamplesPack{
      ctx->generator.convert(audiopacket.audio_data, audiopacket.frames_read)});
}
