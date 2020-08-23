/*
 * Based on: https://github.com/ali1234/raspi-teletext/blob/master/tvctl.c
 */

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <memory>
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "CLI/CLI.hpp"

#include "constants.h"

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

struct Options {
  std::string command;
  int shift;
};

typedef enum { PAL_OFF, PAL_ON, NTSC_OFF, NTSC_ON, UNKNOWN } tstate;

// each register is two shorts

// eg: reg[5] = 0014 0003 - means 0x14 lines then 0x3 lines above the visible
//     reg[6] = 0002 0120 - means 0x120 visible lines, then 0x2 lines below
//     the display add them all you get 313 for odd field and 312 for even
//     field, for 625 total

// therefore, reducing the two values in reg[5] and increasing the first value
// in reg[6] will shift the visible lines up.

// question: what is the difference between the two values in reg[5]/reg[7]
// register?

template <typename... Args>
static std::string string_format(const std::string &format, Args... args) {
  size_t size =
      snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
  if (size <= 0) {
    throw std::runtime_error("Error during formatting.");
  }
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1); // We don't want the '\0' inside
}

static std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
generate_reg_values(const tstate &state, int shift) {
  switch (state) {
  case PAL_OFF:
  case PAL_ON:
    if (shift > 0x14) {
      throw std::string("PAL: can't shift more then 19 lines up");
    }
    return std::make_tuple<uint32_t, uint32_t, uint32_t, uint32_t>(
        ((0x0014 - shift) << 16) | 0x0003, ((0x0002 + shift) << 16) | 0x0120,
        ((0x0013 - shift) << 16) | 0x0003, ((0x0002 + shift) << 16) | 0x0120);

    break;
  case NTSC_OFF:
  case NTSC_ON:
    if (shift > 0x14) {
      throw std::string("NTSC: can't shift more then 16 lines op");
    }
    return std::make_tuple<uint32_t, uint32_t, uint32_t, uint32_t>(
        ((0x0010 - shift) << 16) | 0x0003, ((0x0003 + shift) << 16) | 0x00f0,
        ((0x0010 - shift) << 16) | 0x0003, ((0x0004 + shift) << 16) | 0x00f0);

  default:
    return std::make_tuple<uint32_t, uint32_t, uint32_t, uint32_t>(0, 0, 0, 0);
  }
}

static std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
generate_default_state(const tstate &state) {
  switch (state) {
  case PAL_OFF:
  case PAL_ON:
    return std::make_tuple<uint32_t, uint32_t, uint32_t, uint32_t>(
        0x00140003, 0x00020120, 0x00130003, 0x00020120);

  case NTSC_OFF:
  case NTSC_ON:
    return std::make_tuple<uint32_t, uint32_t, uint32_t, uint32_t>(
        0x00100003, 0x000300f0, 0x00100003, 0x000400f0);

  default:
    return std::make_tuple<uint32_t, uint32_t, uint32_t, uint32_t>(0, 0, 0, 0);
  }
}

static tstate detect_current_state(volatile unsigned int *regs,
                                   const int shift) {
  const auto posible_pal = generate_reg_values(PAL_ON, shift);
  const auto posible_ntsc = generate_reg_values(NTSC_ON, shift);

  const auto default_pal = generate_default_state(PAL_ON);
  const auto default_ntsc = generate_default_state(NTSC_ON);

  const auto state = std::make_tuple<uint32_t, uint32_t, uint32_t, uint32_t>(
      (uint32_t)regs[5], (uint32_t)regs[6], (uint32_t)regs[7],
      (uint32_t)regs[8]);

  if (state == posible_pal) {
    return PAL_ON;
  } else if (state == posible_ntsc) {
    return NTSC_ON;
  } else if (state == default_pal) {
    return PAL_ON;
  } else if (state == default_ntsc) {
    return NTSC_OFF;
  } else {
    return UNKNOWN;
  }
}

static bool
set_state(volatile unsigned int *regs,
          const std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> &state) {
  std::tie(regs[5], regs[6], regs[7], regs[8]) = state;
  return true;
}

bool try_set_regs(volatile unsigned int *regs, const Options &options) {

  tstate state = detect_current_state(regs, options.shift);

  if (state == UNKNOWN) {
    return false;
  }

  (options.command == "on")
      ? set_state(regs, generate_reg_values(state, options.shift))
      : set_state(regs, generate_default_state(state));
  return true;
}

static void configureArgumentParcer(CLI::App &app, Options &options) {
  app.add_option("command", options.command, "Control command (on/off)")
      ->expected(1)
      ->required()
      ->check([](const std::string &s) -> std::string {
        return (s == "on" || s == "off")
                   ? ""
                   : "only \"on\" or \"off\" values posible";
      });
  app.add_flag("-s,--shift", options.shift,
               string_format("Shift up lines: (PAL=%d/NTSC=%d)",
                             constants::PAL_SHIFT_DEFAULT,
                             constants::NTSC_SHIFT_DEFAULT));
}

static int open_mem() {
  int fd = open("/dev/mem", O_RDWR | O_SYNC);

  if (fd == -1) {
    throw std::string("Error opening /dev/mem. Are you forget sudo?\n");
  }
  return fd;
}

static int try_configure(int fd, const Options &opts) {
  volatile unsigned int *map_base;

  /* Map TV page */
  map_base = static_cast<volatile unsigned int *>(
      mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x20807000));

  if (map_base == (void *)-1) {
    throw std::string("Error mapping register memory 0x20807000.\n");
  }

  if (!try_set_regs(map_base, opts)) {

    map_base = static_cast<volatile unsigned int *>(
        mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3f807000));

    if (map_base == (void *)-1) {
      throw std::string("Error mapping register memory 0x3f807000.\n");
    }

    if (!try_set_regs(map_base, opts)) {

      map_base = static_cast<volatile unsigned int *>(mmap(
          0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xfec12000));

      if (map_base == (void *)-1) {
        throw std::string("Error mapping register memory 0xfec12000.\n");
      }

      if (!try_set_regs(map_base, opts)) {
        fprintf(stderr, "Could not find registers. Make sure composite video "
                        "out is enabled.\n");
      }
    }
  }
}

int main(int argc, char **argv) {
  CLI::App app{"Raspberry Pi Framebuffer shiter"};
  Options options;

  configureArgumentParcer(app, options);
  CLI11_PARSE(app, argc, argv);

  auto mem = open_mem();

  try_configure(mem, options);

  close(mem);
  return 0;
}
