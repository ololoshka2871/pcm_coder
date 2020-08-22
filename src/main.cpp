#include <csignal>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <csignal>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "print_duration.h"

#include "audioreader.h"
#include "samplegenerator.h"

#include "PCMFinalStage.h"
#include "frameextender.h"
#include "pcmfrmagemanager.h"
#include "pcmlinegenerator.h"

#include "progresscpp/ProgressBar.hpp"

#include "ffmpegvideocoder.h"

#ifdef PLAYER
#include "Player.h"
#include "sdl2display.h"
#endif

#include "argparser.h"
#include "ctrlc_listener.h"

static bool terminate_flag = false;

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

  // manager->start();

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

  AudioReader audioReader{options.InputFile};

  audioReader.dumpFileInfo(std::cout);

  auto queuesSize = options.Play() ? 1 : 5;

  const AudioSample<int16_t> *audio_data;
  uint32_t frames_read;
  std::chrono::nanoseconds timestamp;

  SampleGenerator gen{options.width14, options.use_dither};

  auto consmer = createConsumerThread(options, queuesSize);
  auto manager = createLineManager(options, consmer->getQueue(), queuesSize);
  PCMLineGenerator lineGenerator{
      /*manager->getInputQueue()*/ manager->getLineProcessor()};
  lineGenerator.set14BitMode(options.width14)
      .setGenerateP(options.parity)
      .setGenerateQ(options.m_Q);

#ifdef PLAYER
  PaError res;
  if (options.Play()) {
    res = Pa_Initialize();
    if (res != paNoError) {
      std::cout << "Can't init PA (" << res << ")" << std::endl;
      return 1;
    }
  }
#endif

  std::cout << "Start " << (options.Play() ? "playing..." : "encoding...")
            << std::endl;

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                      audioReader.duration())
                      .count();
  auto progressBar = initProgressBar(duration);

  {
#ifdef PLAYER
    std::unique_ptr<Player> player;
    if (options.Play()) {
      player = std::make_unique<Player>(0, queuesSize,
                                        AudioReader::output_sample_rate);
    }
#endif

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

#ifdef PLAYER
      if (options.Play()) {
        player->play(audio_data->all, frames_read * 2, 0.5);
      }
#endif

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

#ifdef PLAYER
  if (options.Play()) {
    Pa_Terminate();
  }
#endif

  return 0;
}
