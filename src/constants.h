#ifndef CONSTANTS_H
#define CONSTANTS_H

struct constants {
  static constexpr int PAL_SHIFT_DEFAULT = 25;
  static constexpr int NTSC_SHIFT_DEFAULT = 35;

  static constexpr int getDefaultShift(bool PAL) {
    return PAL ? PAL_SHIFT_DEFAULT : NTSC_SHIFT_DEFAULT;
  }
};

#endif // CONSTANTS_H
