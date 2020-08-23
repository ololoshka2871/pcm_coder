#ifndef PLAYERCONSUMER_H
#define PLAYERCONSUMER_H

#include "IConsumer.h"

#include "AudioProdusser.h"

#include "Player.h"

struct PlayerConsumer : public IConsumer<AudioProdusser::AudioPacket> {
  PlayerConsumer(int32_t outpul_index, uint32_t buf_size);

  void Ressive(const AudioProdusser::AudioPacket &packet) override;

  static int initSound();
  static void destroySound();

private:
  std::unique_ptr<Player> player;
};

#endif // PLAYERCONSUMER_H
