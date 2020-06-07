
#include <csignal>
#include <iostream>
#include <string>

#include "print_duration.h"

#include "audioreader.h"
#include "samplegenerator.h"

#include "pcmencoder.h"
#include "pcmfrmagemanager.h"
#include "pcmlinegenerator.h"

#include "Player.h"

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
        ->expected(0);
  }

  static const char uncompressed[];

  std::string codec{uncompressed};
  bool pal = true;
  bool width14 = true;
  bool use_dither = true;
  bool parity = true;
  bool Q = true;
  bool Cut = false;
  bool Play = false;
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
       << "\tGenerate parity: " << printBool(parity) << endl
       << "Play sound: " << printBool(Play) << endl;
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

static bool terminate_flag = false;

static Options configureArgumentParcer(CLI::App &app) {
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
  Options::newFlag(app, "--play-sound", options.Play,
                   "Play sound while converting");
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

  return options;
}

static void configure_ctrlc_listener() {
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = [](int) { terminate_flag = true; };
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, nullptr);
}

static auto createEncoderThread(const Options &options) {
  auto encoder = std::make_unique<PCMEncoder>(options.OutputFile, options.codec,
                                              options.bitrate, options.Cut);

  encoder->start();

  return encoder;
}

static auto createLineManager(const Options &options,
                              LockingQueue<PCMFrame> &outQueue) {
  auto manager = std::make_unique<PCMFrmageManager>(outQueue);

  manager->start();

  return manager;
}

static void separator(std::ostream &os) { os << std::endl; }

int main(int argc, char *argv[]) {
  configure_ctrlc_listener();

  CLI::App app{"PCM encoder"};
  auto options = configureArgumentParcer(app);
  CLI11_PARSE(app, argc, argv);
  options.dump(std::cout);

  separator(std::cout);

  AudioReader audioReader{options.InputFile};

  audioReader.dumpFileInfo(std::cout);

  const AudioSample<float> *audio_data;
  uint32_t frames_read;
  std::chrono::nanoseconds timestamp;

  SampleGenerator gen{options.width14, options.use_dither};

  auto encoder = createEncoderThread(options);
  auto manager = createLineManager(options, encoder->getQueue());
  PCMLineGenerator lineGenerator{manager->getInputQueue()};

  auto res = Pa_Initialize();
  if (res != paNoError) {
    std::cout << "Can't init PA (" << res << ")" << std::endl;
    return 1;
  }

  {
    std::unique_ptr<Player> player;
    if (options.Play) {
      player = std::make_unique<Player>(0, 10, AudioReader::output_sample_rate);
    }

    while (audioReader.getNextAudioData(audio_data, frames_read, timestamp)) {
      std::cout << "Input: Decodec " << frames_read
                << " frames, TIMESATMP: " << timestamp << std::endl;
      auto res = gen.convert(audio_data, frames_read);

      lineGenerator.input(res);

      if (options.Play) {
        player->play(res.data()->all, frames_read * 2, 0.5);
      }

      if (terminate_flag) {
        std::cout << "Keyboard interrupt!" << std::endl;
        break;
      }
    }
  }

  lineGenerator.flush();

  manager->finalise().join();
  encoder->finalise().join();

  Pa_Terminate();

  return 0;
}
