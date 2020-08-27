#include <iostream>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <csignal>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "ctrlc_listener.h"

static bool *terminate_flag = nullptr;

void configure_ctrlc_listener(bool &flag) {
  terminate_flag = &flag;

#ifdef _MSC_VER
  // https://stackoverflow.com/questions/18291284/handle-ctrlc-on-win32
  if (!SetConsoleCtrlHandler(
          [](DWORD) {
            if (terminate_flag) {
              *terminate_flag = true;
            }
            return TRUE;
          },
          TRUE)) {
    std::cout << "ERROR: Could not set control handler" << std::endl;
    exit(1);
  }
#else
  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = [](int) {
    if (terminate_flag) {
      *terminate_flag = true;
    }
  };
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, nullptr);
#endif
}
