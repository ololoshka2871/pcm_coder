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
#include "RPIFbDisplayConsumer.h"
#include "SDL2DisplayConsumer.h"

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

  AudioProdusser audioprodusser{options.InputFile};

  audioprodusser.reader().dumpFileInfo(std::cout);

  auto &stage =
      audioprodusser
          .NextStage(new BitWidthConverter{options.width14, options.use_dither})
          .NextStage(new LineGeneratorStage{options.width14, options.parity,
                                            options.generateQ()})
          .NextStage(new PCMFrmageStage(options.width14, options.parity,
                                        options.generateQ(),
                                        options.copyProtection, options.pal))
          .NextStage(new PixelDuplicatorStage(
              options.pal, options.crop_top, options.crop_bot,
              FFmpegVideoCoder::PIXEL_WIDTH / PCMFrame::PIXEL_WIDTH));

  if (options.Play()) {
    if (true) {
      auto sdl2display = new SDL2DisplayConsumer();
      sdl2display->onClose([]() { terminate_flag = true; });
      sdl2display->InitRenderer(
          PCMFrame::PIXEL_WIDTH *
              (FFmpegVideoCoder::PIXEL_WIDTH / PCMFrame::PIXEL_WIDTH),
          PixelDuplicatorStage::FrameHeigth(options.pal, options.crop_top,
                                            options.crop_bot));
      stage.NextConsumer(sdl2display);
    } else {
      stage.NextConsumer(new RPIFbDisplayConsumer());
    }
  } else {
    stage.NextConsumer(new FFmpegVideoCoderConsumer());
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
