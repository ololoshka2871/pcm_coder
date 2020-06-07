#include "pcmline.h"

uint16_t PCMLine::generateP() const { return 1; }

uint16_t PCMLine::generateQ() const { return 2; }

std::pair<uint16_t, uint16_t> PCMLine::generatePQ() { return {3, 4}; }
