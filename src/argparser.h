#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>

namespace CLI {
class App;
}

struct Options {
  static const char uncompresed[];

  std::string codec{uncompresed};
  bool pal = true;
  bool width14 = true;
  bool use_dither = true;
  bool parity = true;
  bool m_Q = true;
  bool copyProtection = false;
#ifdef PLAYER
  bool rpiMode = false;
#endif

  uint32_t crop_top = 0;
  uint32_t crop_bot = 0;

  uint32_t bitrate = 15000;
  std::string InputFile;
  std::string OutputFile;

#ifdef RPI
  int Rpi_vsync_delay = 2000;
  int Rpi_left_offset = 0;
  int Rpi_right_offset = 0;
  int Rpi_heigth_mod = 0;
#endif

  std::string formatsStr() const { return pal ? "PAL" : "NTSC"; }
  std::string bitWidthsStr() const { return width14 ? "14 bit" : "16 bit"; }

  bool generateQ() const { return m_Q & width14; }

  bool Play() const {
#ifdef PLAYER
    return OutputFile.empty();
#else
    return false;
#endif
  }

  void dump(std::ostream &os) const;
};

int parseArguments(int argc, char *argv[], Options &options);

#endif // ARGPARSER_H
