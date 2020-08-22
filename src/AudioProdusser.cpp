

#include "AudioProdusser.h"

AudioProdusser::AudioProdusser(const std::string &filename)
    : audioreader(filename) {}

Result<std::chrono::nanoseconds, bool> AudioProdusser::exec() {
  AudioPacket packet;
  std::chrono::nanoseconds timestamp;

  if (!audioreader.getNextAudioData(packet.audio_data, packet.frames_read,
                                    timestamp)) {
    audioDataconsumer->Ressive(AudioPacketEof());
    return Err(false);
  }

  audioDataconsumer->Ressive(packet);
  return Ok(timestamp);
}
