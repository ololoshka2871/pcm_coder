#include <iomanip>
#include <optional>

#include "print_duration.h"

// https://stackoverflow.com/a/58127222
std::ostream &operator<<(std::ostream &os, std::chrono::nanoseconds ns) {
  using namespace std::chrono;
  using days = duration<int, std::ratio<86400>>;
  auto d = duration_cast<days>(ns);
  ns -= d;
  auto h = duration_cast<hours>(ns);
  ns -= h;
  auto m = duration_cast<minutes>(ns);
  ns -= m;
  auto s = duration_cast<seconds>(ns);
  ns -= s;

  std::optional<int> fs_count;
  switch (os.precision()) {
  case 9:
    fs_count = ns.count();
    break;
  case 6:
    fs_count = duration_cast<microseconds>(ns).count();
    break;
  case 3:
    fs_count = duration_cast<milliseconds>(ns).count();
    break;
  }

  char fill = os.fill('0');
  if (d.count())
    os << d.count() << "d ";
  if (d.count() || h.count())
    os << std::setw(2) << h.count() << ":";
  if (d.count() || h.count() || m.count())
    os << std::setw(d.count() || h.count() ? 2 : 1) << m.count() << ":";
  os << std::setw(d.count() || h.count() || m.count() ? 2 : 1) << s.count();
  if (fs_count.has_value())
    os << "." << std::setw(os.precision()) << fs_count.value();
  if (!d.count() && !h.count() && !m.count())
    os << "s";

  os.fill(fill);
  return os;
}
