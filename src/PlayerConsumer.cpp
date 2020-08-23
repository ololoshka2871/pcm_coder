#include <iostream>

#include "PlayerConsumer.h"

PlayerConsumer::PlayerConsumer(int32_t outpul_index, uint32_t buf_size)
    : player{std::make_unique<Player>(outpul_index, buf_size,
                                      AudioReader::output_sample_rate)} {}

void PlayerConsumer::Ressive(const AudioProdusser::AudioPacket &packet) {
  if (!player) {
    return;
  }

  if (packet.eof()) {
    player.reset();
    return;
  }

  player->play(packet.audio_data->all, packet.frames_read * 2, 0.5);
}

int PlayerConsumer::initSound() {
  auto res = Pa_Initialize();
  if (res != paNoError) {
    std::cout << "Can't init PA (" << res << ")" << std::endl;
    return 1;
  }
  return 0;
}

void PlayerConsumer::destroySound() { Pa_Terminate(); }
