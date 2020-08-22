#ifndef AUDIOPRODUSSER_H
#define AUDIOPRODUSSER_H

#include <memory>

#include "result.h"

#include "IConsumer.h"

#include "audioreader.h"

#include "abstractprocessingstage.h"

struct AudioProdusser {
  struct AudioPacket {
    const AudioSample<int16_t> *audio_data;
    uint32_t frames_read;

    virtual bool eof() const { return false; }
  };

  struct AudioPacketEof : public AudioPacket {
    bool eof() const override { return true; }
  };

  const AudioReader &reader() const { return audioreader; }

  AudioProdusser(const std::string &filename);

  IConsumer<AudioPacket> &NextConsumer(IConsumer<AudioPacket> *newconsumer) {
    audioDataconsumer.reset(newconsumer);
    return *newconsumer;
  }

  template <typename To>
  AbstractprocessingStage<AudioPacket, To> &
  NextStage(AbstractprocessingStage<AudioPacket, To> *newconsumer) {
    audioDataconsumer.reset(newconsumer);
    return *newconsumer;
  }

  Result<std::chrono::nanoseconds, bool> exec();

  std::chrono::nanoseconds duration() const { return audioreader.duration(); }

private:
  std::unique_ptr<IConsumer<AudioPacket>> audioDataconsumer;
  AudioReader audioreader;
};

#endif // AUDIOPRODUSSER_H
