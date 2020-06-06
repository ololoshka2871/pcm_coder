#ifndef PRINT_DURATION_H
#define PRINT_DURATION_H

#include <chrono>
#include <ostream>

std::ostream &operator<<(std::ostream &os, std::chrono::nanoseconds ns);

#endif // PRINT_DURATION_H
