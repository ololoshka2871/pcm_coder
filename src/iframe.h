#ifndef IFRAME_H
#define IFRAME_H

#include <cstdint>
#include <vector>

struct IFrame {
  static constexpr uint8_t default_gray_lvl = 150;
  static constexpr uint8_t default_white_lvl = 255;

  enum {
    SYNC_LINE_1 = 1,
    SYNC_LINE_2 = 3,
    DATA_OFFSET_PIXELS = 5,

    WHITE_LINE = 134,
    WHITE_WIDTH = 4
  };

  IFrame(size_t width, size_t heigth) : m_width{width}, m_heigth{heigth} {}

  virtual ~IFrame() {}

  virtual std::vector<uint8_t>
  toPixels(uint8_t grayLevel = default_gray_lvl,
           uint8_t white_lvl = default_white_lvl) const = 0;

  virtual size_t width() const { return m_width; }
  virtual size_t heigth() const { return m_heigth; }

protected:
  size_t m_width, m_heigth;
};

#endif // IFRAME_H
