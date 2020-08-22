#include "BitWidthConverter.h"

BitWidthConverter::BitWidthConverter(bool is_14_bit, bool use_dither)
    : generator{is_14_bit, use_dither} {}

void BitWidthConverter::Ressive(
    const AudioProdusser::AudioPacket &audiopacket) {
  if (audiopacket.eof()) {
    Send(SamplesPackEof{});
    return;
  }
  Send(SamplesPack{
      generator.convert(audiopacket.audio_data, audiopacket.frames_read)});
}
