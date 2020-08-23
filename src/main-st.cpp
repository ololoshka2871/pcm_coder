#ifdef _MSC_VER
#include <windows.h>
#else
#include <csignal>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "progresscpp/ProgressBar.hpp"

#include "AudioProdusser.h"
#include "BitWidthConverter.h"
#include "FFmpegVideoCoderConsumer.h"
#include "LineGeneratorStage.h"
#include "PCMFrmageStage.h"
#include "PixelDuplicatorStage.h"
#include "Splitter.h"

#ifdef PLAYER
#include "PlayerConsumer.h"
#include "RPIFbDisplayConsumer.h"
#include "SDL2DisplayConsumer.h"

#include "constants.h"
#endif

#include "ffmpegvideocoder.h"

#include "ctrlc_listener.h"

#include "argparser.h"

static bool terminate_flag = false;

static void separator(std::ostream &os) { os << std::endl; }

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

  if (cols <= 0 || cols > 200) {
    cols = 80;
  }
#endif

  return progresscpp::ProgressBar{limit, static_cast<uint32_t>(cols - 20)};
}

static int play(Options &options) {
  AudioProdusser audioprodusser{options.InputFile};

  audioprodusser.reader().dumpFileInfo(std::cout);

#ifdef PLAYER
  if (options.Play()) {
    if (options.rpiMode) {
      SDL2DisplayConsumerBase::VideoInit();
      try {
        options.pal = SDL2DisplayConsumerBase::DetectPALNTSC();

        // mihail2501: для NTSC 24-26 строк (где-то между) для пал 34-36 строк
        // точное значение надо на дидовьем :pcm:  замерить
        const auto z = constants::getDefaultShift(options.pal);

        std::cout << "Raspberry Pi playing mode detected: "
                  << options.formatsStr() << std::endl
                  << "For correct playing shift Visable Region up by " << z
                  << " lines!" << std::endl
                  << "Example: $ sudo rpi-fb-shifter " << z << std::endl;
      } catch (...) {
        auto [w, h] = SDL2DisplayConsumerBase::getDisplaySize();
        std::cerr << "Failed to detect Raspberry Pi output mode! (" << w << "x"
                  << h << ")" << std::endl;
        return 1;
      }
    } else {
      if (PlayerConsumer::initSound() != 0) {
        std::cerr << "Failed to initialise sound system!" << std::endl;
        return 1;
      }
    }
  }
#endif

  // --- working pipeline ---

  Splitter<AudioProdusser::AudioPacket> *splitter = nullptr;
  if (options.Play() && !options.rpiMode) {
#if PLAYER
    splitter = new Splitter<AudioProdusser::AudioPacket>();
    splitter->AddConsumer(new PlayerConsumer(0, 2));
#endif
  }

  auto bitWidthConverter =
      new BitWidthConverter{options.width14, options.use_dither};

  auto &preparedSamples =
      (options.Play() && !options.rpiMode)
          ? audioprodusser.NextStage(splitter).NextStage(bitWidthConverter)
          : audioprodusser.NextStage(bitWidthConverter);

  auto &stage =
      preparedSamples
          .NextStage(new LineGeneratorStage{options.width14, options.parity,
                                            options.generateQ()})
          .NextStage(new PCMFrameStage(options.width14, options.parity,
                                       options.generateQ(),
                                       options.copyProtection, options.pal));

  if (options.Play()) {
#ifdef PLAYER
    SDL2DisplayConsumerBase *display;
    if (options.rpiMode) {
      display = new RPIFbDisplayConsumer();
      std::apply(
          [&display](auto &&... args) { display->InitRenderer(args...); },
          SDL2DisplayConsumer::getDisplaySize());
    } else {
      display = new SDL2DisplayConsumer();
      display->InitRenderer(
          FFmpegVideoCoder::PIXEL_WIDTH,
          PixelDuplicatorStage::FrameHeigth(options.pal, options.crop_top,
                                            options.crop_bot));
    }
    display->onClose([]() { terminate_flag = true; });
    stage.NextConsumer(display);
#endif
  } else {
    stage
        .NextStage(new PixelDuplicatorStage(
            options.pal, options.crop_top, options.crop_bot,
            FFmpegVideoCoder::PIXEL_WIDTH / PCMFrame::PIXEL_WIDTH))
        .NextConsumer(new FFmpegVideoCoderConsumer(
            options.OutputFile, options.codec, options.bitrate, options.pal));
  }

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                      audioprodusser.duration())
                      .count();
  auto progressBar = initProgressBar(duration);

  // --- Грязный хак ---
  unsigned int &progress_value =
      *reinterpret_cast<unsigned int *>(&progressBar);

  // -- play --
  while (!terminate_flag) {
    auto timestamp = audioprodusser.exec();

    if (timestamp.isErr()) {
      progress_value = duration;
      break;
    }

    progress_value = std::chrono::duration_cast<std::chrono::microseconds>(
                         timestamp.unwrap())
                         .count();

    progressBar.display();
  }

  progressBar.done();

  return 0;
}

int main(int argc, char *argv[]) {
  configure_ctrlc_listener(terminate_flag);

  Options options;

  {
    auto ret = parseArguments(argc, argv, options);
    if (ret) {
      return ret;
    }
  }

  options.dump(std::cout);

  separator(std::cout);

  auto ret = play(options);

#ifdef PLAYER
  if (options.Play() && !options.rpiMode) {
    PlayerConsumer::destroySound();
  }
#endif

  return ret;
}
