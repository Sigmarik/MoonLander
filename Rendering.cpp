#include "Rendering.h"

#include <assert.h>
#include <iostream>
#include <math.h>
#include <string.h>

void Screen::flush() {
  memset(buffer_, 0, width_ * height_ * sizeof(*buffer_));
}

bool Screen::setAt(long long x, long long y, IntColor color) {
  assert(buffer_);

  if (x < 0 || y < 0) {
    return false;
  }

  if (x >= width_ || y >= height_) {
    return false;
  }

  buffer_[y * width_ + x] = *(uint32_t *)&color;

  return true;
}

static long long sign(long long value) {
  if (value > 0)
    return 1;
  if (value < 0)
    return -1;
  return 0;
}

void Screen::drawLine(const Line &line, IntColor color, Flags flags) {
  Line local_line = worldToScreen(line, flags);

  long long start_x = local_line.start.x, start_y = local_line.start.y;
  long long end_x = local_line.end.x, end_y = local_line.end.y;
  long long delta_x = end_x - start_x, delta_y = end_y - start_y;

  bool swap_directions = delta_x * delta_y < 0;

  long long current_x = start_x, current_y = start_y;

  for (size_t step_id = 0; step_id < abs(delta_x) + abs(delta_y); ++step_id) {
    long long steer =
        (current_x - start_x) * delta_y - (current_y - start_y) * delta_x;

    setAt(current_x, current_y, color);

    if (steer == 0) {
      if (abs(delta_x) >= abs(delta_y)) {
        current_x += sign(delta_x);
      } else {
        current_y += sign(delta_y);
      }
    } else if ((steer > 0) ^ swap_directions) {
      // Turn left

      current_y += sign(delta_y);
    } else if ((steer < 0) ^ swap_directions) {
      // Turn right

      current_x += sign(delta_x);
    }
  }
}

Vec2f Screen::worldToScreen(const Vec2f &point, Flags flags) const {
  Vec2f result = point;

  if (!(flags & DrawFlags::IGNORE_SHIFT)) {
    result -= shift_;
  }

  if (!(flags & DrawFlags::IGNORE_SCALE)) {
    result *= scale_;
  }

  result = result * dots_per_unit_ + Vec2f(width_, height_) / 2.0;
  result.y = height_ - result.y;
  return result;
}

Line Screen::worldToScreen(const Line &line, Flags flags) const {
  Line result{};

  result.start = worldToScreen(line.start, flags);
  result.end = worldToScreen(line.end, flags);

  Vec2f center_projection =
      result.start +
      result.dir() * dot(result.dir(), -result.start) / result.length2();

  float max_distance = 2 * (width_ * width_ + height_ * height_);

  if (center_projection.length2() > max_distance) {
    return Line();
  }

  if (result.start.length2() > max_distance) {
    result.start =
        center_projection +
        (result.start - center_projection).normalized() * sqrt(max_distance);
  }

  if (result.end.length2() > max_distance) {
    result.end =
        center_projection +
        (result.end - center_projection).normalized() * sqrt(max_distance);
  }

  return result;
}
