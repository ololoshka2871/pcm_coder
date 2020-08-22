#ifndef BITWIDTHCONVERTER_H
#define BITWIDTHCONVERTER_H

#include "abstractprocessingstage.h"

#include "AudioProdusser.h"

#include "samplegenerator.h"

#include "samplespack.h"

struct BitWidthConverter
    : public AbstractprocessingStage<AudioProdusser::AudioPacket, SamplesPack> {

  BitWidthConverter(bool is_14_bit = false, bool use_dither = false);

  void Ressive(const AudioProdusser::AudioPacket &audiopacket) override;

private:
  SampleGenerator generator;
};

#endif // BITWIDTHCONVERTER_H
