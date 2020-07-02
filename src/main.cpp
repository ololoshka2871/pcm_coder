
#include <csignal>
#include <iostream>
#include <string>

#include "print_duration.h"

#include "audioreader.h"
#include "samplegenerator.h"

#include "PCMFinalStage.h"
#include "frameextender.h"
#include "pcmfrmagemanager.h"
#include "pcmlinegenerator.h"

#include "progresscpp/ProgressBar.hpp"

#ifdef _MSC_VER
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "ffmpegvideocoder.h"

#ifdef PLAYER
#include "Player.h"
#include "sdl2display.h"
#endif

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

  static const char uncompresed[];

  std::string codec{uncompresed};
  bool pal = true;
  bool width14 = true;
  bool use_dither = true;
  bool parity = true;
  bool m_Q = true;
  bool copyProtection = false;

  uint32_t crop_top = 0;
  uint32_t crop_bot = 0;

  uint32_t bitrate = 15000;
  std::string InputFile;
  std::string OutputFile;

  std::string formatsStr() const { return pal ? "PAL" : "NTSC"; }
  std::string bitWidthsStr() const { return width14 ? "14 bit" : "16 bit"; }
  static std::string printBool(bool v) { return v ? "YES" : "NO"; }

  bool generateQ() const { return m_Q & width14; }

  bool Play() const { return OutputFile.empty(); }

  void dump(std::ostream &os) const {
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
};

template <>
auto Options::newOption<std::string>(CLI::App &app,
                                     const std::string &option_name,
                                     std::string &value,
                                     const std::string &description) {
  return app
      .add_option(option_name, value, description + " [Default: " + value + "]")
      ->expected(1);
}

template <>
std::string Options::generate_default_str<bool>(std::string &default_str,
                                                const bool &val) {
  return default_str.empty() ? " [Default: " + Options::printBool(val) + "]"
                             : " [Default: " + default_str + "]";
}

const char Options::uncompresed[] = "rawvideo";

static bool terminate_flag = false;

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

  auto codec_opt =
      Options::newOption(app, "-c,--video-codec", options.codec,
                         "Video codec to compress result (for FFmpeg).")
          ->needs(output);

  Options::newFlag(app, "--pal,!--ntsc", options.pal,
                   "Output video format: PAL/NTSC.", options.formatsStr());
  Options::newFlag(app, "--14,!--16", options.width14,
                   "Output bit widtht: 14/16 bit. 16BIT NOT WORKING YET",
                   options.bitWidthsStr());
  Options::newFlag(app, "--with-dither,!--no-dither,!--ND", options.use_dither,
                   "Use dither when convering 16 bit -> 14 bit.");
  Options::newFlag(app, "--with-parity,!--no-parity,!--NP", options.parity,
                   "Generate parity.");
  Options::newFlag(app, "--with-q,!--no-q,!--NQ", options.m_Q, "Generate Q.");
  Options::newFlag(app, "--copy-protection,--CP,!--no-copy-protection",
                   options.copyProtection, "Set copy protection bit.");
  Options::newOption(app, "-b,--video-bitrate", options.bitrate,
                     "Set video bitrate (if not uncompressed).")
      ->needs(output)
      ->needs(codec_opt);
  Options::newOption(app, "--crop-top", options.crop_top,
                     "Crop N lines from TOP of frame.");
  Options::newOption(app, "--crop-bot", options.crop_bot,
                     "Crop N lines from BOTTOM of frame.");
}

static void configure_ctrlc_listener() {
#ifdef _MSC_VER
  // https://stackoverflow.com/questions/18291284/handle-ctrlc-on-win32
  if (!SetConsoleCtrlHandler(
          [](DWORD) {
            terminate_flag = true;
            return TRUE;
          },
          TRUE)) {
    std::cout << "ERROR: Could not set control handler" << std::endl;
    exit(1);
  }
#else
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = [](int) { terminate_flag = true; };
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, nullptr);
#endif
}

static auto createConsumerThread(const Options &options, uint32_t queueSize) {
  static constexpr auto Width = 139; // fixme

  auto consumer = std::make_unique<PCMFinalStage>(
      Width, PCMFrmageManager::getHeigth(options.pal), queueSize);

  auto pixeldublicator = std::make_unique<FrameExtender>(
      options.pal, options.crop_top, options.crop_bot,
      FFmpegVideoCoder::PIXEL_WIDTH / PCMFrame::PIXEL_WIDTH);

#ifdef PLAYER
  if (options.Play()) {
    pixeldublicator->setConsumer(std::make_unique<SDL2Display>(
        []() { terminate_flag = true; }, queueSize));
  } else
#endif
  {
    pixeldublicator->setConsumer(std::make_unique<FFmpegVideoCoder>(
        options.OutputFile, options.codec, options.bitrate, options.pal));
  }

  consumer->setPolicy(std::move(pixeldublicator));

  consumer->start();

  return consumer;
}

static auto createLineManager(const Options &options,
                              LockingQueue<std::unique_ptr<IFrame>> &outQueue,
                              uint32_t quieueSize) {
  auto manager = std::make_unique<PCMFrmageManager>(
      options.width14, options.parity, options.generateQ(),
      options.copyProtection, options.pal, outQueue, quieueSize);

  manager->start();

  return manager;
}

static progresscpp::ProgressBar initProgressBar(uint32_t limit) {
  int cols;

#ifdef _MSC_VER
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int columns, rows;

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
#ifdef TIOCGSIZE
  struct ttysize ts;
  ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
  cols = ts.ts_cols;
#elif defined(TIOCGWINSZ)
  struct winsize ts;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
  cols = ts.ws_col;
#endif /* TIOCGSIZE */

  if (cols == 0 || cols > 200) {
    cols = 80;
  }
#endif

  return progresscpp::ProgressBar{limit, static_cast<uint32_t>(cols - 20)};
}

static void separator(std::ostream &os) { os << std::endl; }

int main(int argc, char *argv[]) {
  configure_ctrlc_listener();

  Options options;

  CLI::App app{"PCM encoder"};
  configureArgumentParcer(app, options);
  CLI11_PARSE(app, argc, argv);
  options.dump(std::cout);

  separator(std::cout);

  AudioReader audioReader{options.InputFile};

  audioReader.dumpFileInfo(std::cout);

  auto queuesSize = options.Play() ? 1 : 5;

  const AudioSample<int16_t> *audio_data;
  uint32_t frames_read;
  std::chrono::nanoseconds timestamp;

  SampleGenerator gen{options.width14, options.use_dither};

  auto consmer = createConsumerThread(options, queuesSize);
  auto manager = createLineManager(options, consmer->getQueue(), queuesSize);
  PCMLineGenerator lineGenerator{manager->getInputQueue()};
  lineGenerator.set14BitMode(options.width14)
      .setGenerateP(options.parity)
      .setGenerateQ(options.m_Q);

  PaError res;
  if (options.Play()) {
    res = Pa_Initialize();
    if (res != paNoError) {
      std::cout << "Can't init PA (" << res << ")" << std::endl;
      return 1;
    }
  }

  std::cout << "Start " << (options.Play() ? "playing..." : "encoding...")
            << std::endl;

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                      audioReader.duration())
                      .count();
  auto progressBar = initProgressBar(duration);

  {
    std::unique_ptr<Player> player;
    if (options.Play()) {
      player = std::make_unique<Player>(0, queuesSize,
                                        AudioReader::output_sample_rate);
    }

    // --- Грязный хак ---
    unsigned int &progress_value =
        *reinterpret_cast<unsigned int *>(&progressBar);

    while (audioReader.getNextAudioData(audio_data, frames_read, timestamp)) {
#if 0
      std::cout << "Input: Decodec " << frames_read
                << " frames, TIMESATMP: " << timestamp << std::endl;
#endif

      auto res = gen.convert(audio_data, frames_read);
      lineGenerator.input(res);

      auto ts_s =
          std::chrono::duration_cast<std::chrono::microseconds>(timestamp)
              .count();

      progress_value = ts_s;

      progressBar.display();

      if (options.Play()) {
        player->play(audio_data->all, frames_read * 2, 0.05);
      }

      if (terminate_flag) {
        progressBar.done();
        std::cout << "Interrupt!" << std::endl;
        goto done;
      }
    }
    progress_value = duration;
    progressBar.done();
  }

done:
  lineGenerator.flush();

  if (options.Play()) {
    Pa_Terminate();
  }

  return 0;
}
