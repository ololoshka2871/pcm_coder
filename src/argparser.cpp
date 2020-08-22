
#include "CLI/CLI.hpp"

#include "argparser.h"

const char Options::uncompresed[] = "rawvideo";

static std::string printBool(bool v) { return v ? "YES" : "NO"; }

template <typename T>
static auto newOption(CLI::App &app, const std::string &option_name, T &value,
                      const std::string &description = "") {
  return app
      .add_option(option_name, value,
                  description + " [Default: " + std::to_string(value) + "]")
      ->expected(1);
}

template <typename T>
static std::string generate_default_str(std::string &default_str,
                                        const T &val) {
  return default_str.empty() ? " [Default: " + std::to_string(val) + "]"
                             : " [Default: " + default_str + "]";
}

template <typename T>
static auto newFlag(CLI::App &app, const std::string &flag_name, T &value,
                    const std::string &description = "",
                    std::string default_val = std::string()) {
  return app
      .add_flag(flag_name, value,
                description + generate_default_str(default_val, value))
      ->expected(0);
}

template <>
auto newOption<std::string>(CLI::App &app, const std::string &option_name,
                            std::string &value,
                            const std::string &description) {
  return app
      .add_option(option_name, value, description + " [Default: " + value + "]")
      ->expected(1);
}

template <>
std::string generate_default_str<bool>(std::string &default_str,
                                       const bool &val) {
  return default_str.empty() ? " [Default: " + printBool(val) + "]"
                             : " [Default: " + default_str + "]";
}

static void configureArgumentParcer(CLI::App &app, Options &options) {
  app.add_option("audiofile", options.InputFile, "Audiofile to process.")
      ->expected(1)
      ->required()
      ->check(CLI::ExistingFile);
  auto output =
      app.add_option("outputfile", options.OutputFile, "Result video file.")
          ->expected(1)
#ifndef PLAYER
          ->required()
#endif
      ;

  auto codec_opt = newOption(app, "-c,--video-codec", options.codec,
                             "Video codec to compress result (for FFmpeg).")
                       ->needs(output);

  newFlag(app, "--pal,!--ntsc", options.pal, "Output video format: PAL/NTSC.",
          options.formatsStr());
  newFlag(app, "--14,!--16", options.width14,
          "Output bit widtht: 14/16 bit. 16BIT NOT WORKING YET",
          options.bitWidthsStr());
  newFlag(app, "--with-dither,!--no-dither,!--ND", options.use_dither,
          "Use dither when convering 16 bit -> 14 bit.");
  newFlag(app, "--with-parity,!--no-parity,!--NP", options.parity,
          "Generate parity.");
  newFlag(app, "--with-q,!--no-q,!--NQ", options.m_Q, "Generate Q.");
  newFlag(app, "--copy-protection,--CP,!--no-copy-protection",
          options.copyProtection, "Set copy protection bit.");
  newOption(app, "-b,--video-bitrate", options.bitrate,
            "Set video bitrate (if not uncompressed).")
      ->needs(output)
      ->needs(codec_opt);
  newOption(app, "--crop-top", options.crop_top,
            "Crop N lines from TOP of frame.");
  newOption(app, "--crop-bot", options.crop_bot,
            "Crop N lines from BOTTOM of frame.");
}

void Options::dump(std::ostream &os) const {
  using namespace std;

  os << "Encoder options:" << endl << "\tInput file: " << InputFile << endl;
  if (!Play()) {
    os << "\tOutput File: " << OutputFile << endl
       << "\tCodec: " << codec << endl;
  }
  os << "\tFormat: " << formatsStr() << endl
     << "\tBit width: " << bitWidthsStr() << endl
     << "\tGenerate parity: " << printBool(parity) << endl
     << "\tAdd copy-protection bit: " << printBool(copyProtection) << endl;
  if (width14) {
    os << "\tGenerate Q: " << printBool(generateQ()) << endl
       << "\tUse dither: " << printBool(width14 ? use_dither : false) << endl;
  }
  if (crop_top || crop_bot) {
    os << "\tCrop video top=" << crop_top << ", bot=" << crop_bot << endl;
  }
  if (codec != uncompresed) {
    os << "\tVideo bitrate: " << bitrate << endl;
  }
}

int parseArguments(int argc, char *argv[], Options &options) {
  CLI::App app{"PCM encoder"};
  configureArgumentParcer(app, options);
  CLI11_PARSE(app, argc, argv);
  return 0;
}
