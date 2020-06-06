
#include <iostream>
#include <string>

#include "print_duration.h"

#include "audioreader.h"

#include "CLI/CLI.hpp"

struct Options {
  template <typename T>
  static auto newOption(CLI::App &app, const std::string &option_name, T &value,
                        const std::string &description = "") {
    return app
        .add_option(option_name, value,
                    description + " [Default: " + std::to_string(value) + "]")
        ->expected(1);
  }

  template <>
  auto newOption<std::string>(CLI::App &app, const std::string &option_name,
                              std::string &value,
                              const std::string &description) {
    return app
        .add_option(option_name, value,
                    description + " [Default: " + value + "]")
        ->expected(1);
  }

  template <typename T>
  static std::string generate_default_str(std::string &default_str,
                                          const T &val) {
    return default_str.empty() ? " [Default: " + std::to_string(val) + "]"
                               : " [Default: " + default_str + "]";
  }

  template <>
  std::string generate_default_str<bool>(std::string &default_str,
                                         const bool &val) {
    return default_str.empty() ? " [Default: " + Options::printBool(val) + "]"
                               : " [Default: " + default_str + "]";
  }

  template <typename T>
  static auto newFlag(CLI::App &app, const std::string &flag_name, T &value,
                      const std::string &description = "",
                      std::string default_val = std::string()) {
    return app
        .add_flag(flag_name, value,
                  description + generate_default_str(default_val, value))
        ->expected(1);
  }

  static const char uncompressed[];

  std::string codec{uncompressed};
  bool pal = true;
  bool width14 = true;
  bool use_dither = true;
  bool parity = true;
  bool Q = true;
  bool Cut = false;
  uint32_t bitrate = 15000;
  std::string InputFile;
  std::string OutputFile;

  std::string formatsStr() const { return pal ? "PAL" : "NTSC"; }
  std::string bitWidthsStr() const { return width14 ? "14 bit" : "16 bit"; }
  static std::string printBool(bool v) { return v ? "YES" : "NO"; }

  void dump(std::ostream &os) const {
    using namespace std;

    os << "Coder options:" << endl
       << "\tInutput file: " << InputFile << endl
       << "\tOutput File: " << OutputFile << endl
       << "\tCodec: " << codec << endl
       << "\tFormat: " << formatsStr() << endl
       << "\tBit width: " << bitWidthsStr() << endl
       << "\tUse diter: " << printBool(width14 ? use_dither : false) << endl
       << "\tGenerate parity: " << printBool(parity) << endl;
    if (width14) {
      os << "\tGenerate Q: " << printBool(Q) << endl;
    }
    os << "\tCut video: " << printBool(Cut) << endl;
    if (codec != uncompressed) {
      os << "\tVideo bitrate: " << bitrate << endl;
    }
  }
};

const char Options::uncompressed[] = "Uncompressed";

int main(int argc, char *argv[]) {

  CLI::App app{"PCM encoder"};

  Options options;

  auto codec_opt =
      Options::newOption(app, "-c,--video-codec", options.codec,
                         "Video codec to compress result (for FFmpeg).");
  Options::newFlag(app, "--pal,!--ntsc", options.pal,
                   "Output video format: PAL/NTSC.", options.formatsStr());
  Options::newFlag(app, "--14,!--16", options.width14,
                   "Output bit widtht: 14/16 bit.", options.bitWidthsStr());
  Options::newFlag(app, "--with-dither,!--no-dither,!--ND", options.use_dither,
                   "Use dither when convering 16 bit -> 14 bit.");
  Options::newFlag(app, "--with-parity,!--no-parity,!--NP", options.parity,
                   "Generate parity.");
  Options::newFlag(app, "--with-q,!--no-q,!--NQ", options.Q, "Generate Q.");
  Options::newFlag(app, "-C,--Cut", options.Cut, "Cut-out invisavle strings.");
  Options::newOption(app, "-b,--video-bitrate", options.bitrate,
                     "Set video bitrate (if not Uncompressed).")
      ->needs(codec_opt);
  app.add_option("audiofile", options.InputFile, "Audiofile to process.")
      ->expected(1)
      ->required()
      ->check(CLI::ExistingFile);
  app.add_option("outputfile", options.OutputFile, "Result video file.")
      ->expected(1)
      ->required();

  CLI11_PARSE(app, argc, argv);

  options.dump(std::cout);

  std::cout << std::endl;

  AudioReader audioReader{options.InputFile};

  audioReader.dumpFileInfo(std::cout);

  const AudioReader::AudioSample *audio_data;
  int frames_read;
  std::chrono::nanoseconds timestamp;

  while (audioReader.getNextAudioData(audio_data, frames_read, timestamp)) {
    std::cout << "Input: Decodec " << frames_read
              << " frames, TIMESATMP: " << timestamp << std::endl;
  }

  return 0;
}
