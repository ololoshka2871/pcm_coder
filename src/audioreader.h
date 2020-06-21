#ifndef AUDIOREADER_H
#define AUDIOREADER_H

#include <chrono>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "audiosample.h"

struct AudioReader {

  static constexpr auto output_sample_rate = 44100;

  using AudioSample = AudioSample<int16_t>;

  AudioReader(const std::string &filename);
  ~AudioReader();

  void dumpFileInfo(std::ostream &os) const;

  std::chrono::nanoseconds duration() const;

  bool getNextAudioData(const AudioSample *&pData, uint32_t &nb_samples,
                        std::chrono::nanoseconds &timestamp);

private:
  struct Context;

  std::unique_ptr<Context> ctx;

  bool read_next_audio_pocket();
  void unref_current_pocket();
};

#endif // AUDIOREADER_H
