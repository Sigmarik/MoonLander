#pragma once

#include "Geometry.h"
#include <inttypes.h>

using Flags = uint32_t;

struct IntColor {
  uint8_t b = 0;
  uint8_t g = 0;
  uint8_t r = 0;
  uint8_t a = 255;

  IntColor() = default;
  IntColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
      : b(b), g(g), r(r), a(a) {}

  IntColor operator*(float value) const {
    return IntColor(r * value, g * value, b * value, a);
  }
};

struct Screen {
  Screen() = default;
  Screen(uint32_t *buffer, unsigned width, unsigned height)
      : buffer_(buffer), width_(width), height_(height) {}

  void flush();

  bool setAt(long long x, long long y, IntColor color = IntColor());

  float getScale() const { return scale_; }

  void setShift(const Vec2f &shift) { shift_ = shift; }
  void setScale(float scale) { scale_ = scale; }

  enum DrawFlags {
    IGNORE_SHIFT = 1,
    IGNORE_SCALE = 2,
  };

  void drawLine(const Line &line, IntColor color, Flags flags = 0);

private:
  Vec2f worldToScreen(const Vec2f &point, Flags flags) const;
  Line worldToScreen(const Line &line, Flags flags) const;

  void drawPure(const Line &line, IntColor color);

  uint32_t *buffer_ = nullptr;
  unsigned width_ = 0, height_ = 0;

  float scale_ = 1.0;
  float dots_per_unit_ = 100.0;

  Vec2f shift_{};
};
