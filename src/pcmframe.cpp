#include "pcmframe.h"

PCMFrame::PCMFrame(size_t heigth) : heigth{heigth}, data(heigth * WIDTH) {}
