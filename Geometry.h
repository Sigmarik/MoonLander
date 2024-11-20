#pragma once

#include <array>
#include <math.h>

//! It would have been better to use something like glm.
//! No bicycle factory building required.

struct Vec2f {
  float x = 0.0, y = 0.0;

  Vec2f() = default;
  explicit Vec2f(float value) { x = y = value; }
  Vec2f(float x, float y) : x(x), y(y) {}

  static Vec2f fromAzimuth(float angle, float amplitude = 1.0f) {
    return Vec2f(amplitude * cos(angle), amplitude * sin(angle));
  }

  float azimuth() {
    float atan = atan2(y, x);
    return atan >= 0.0 ? atan : atan + 2 * M_PI;
  }

  Vec2f ort() const { return Vec2f(-y, x); }

  float length() const { return sqrt(length2()); }
  float length2() const { return x * x + y * y; }

  Vec2f normalized(float threshold = 1e-4) const {
    float len = length();
    if (abs(len) < threshold) {
      return *this;
    }

    return *this / len;
  }

  Vec2f operator-() const { return *this * (-1.0f); }
  void operator+=(const Vec2f &other) { x += other.x, y += other.y; }
  void operator-=(const Vec2f &other) { x -= other.x, y -= other.y; }
  void operator*=(const Vec2f &other) { x *= other.x, y *= other.y; }
  void operator/=(const Vec2f &other) { x /= other.x, y /= other.y; }
  void operator*=(float value) { x *= value, y *= value; }
  void operator/=(float value) { x /= value, y /= value; }

  friend Vec2f operator+(const Vec2f &alpha, const Vec2f &beta);
  friend Vec2f operator-(const Vec2f &alpha, const Vec2f &beta);
  friend Vec2f operator*(const Vec2f &alpha, const Vec2f &beta);
  friend Vec2f operator/(const Vec2f &alpha, const Vec2f &beta);
  friend Vec2f operator*(const Vec2f &alpha, float value);
  friend Vec2f operator/(const Vec2f &alpha, float value);

  friend float dot(const Vec2f &alpha, const Vec2f &beta);
  friend float cross(const Vec2f &alpha, const Vec2f &beta);
  friend float dist(const Vec2f &alpha, const Vec2f &beta);
  friend float dist2(const Vec2f &alpha, const Vec2f &beta);
};

struct Circle {
  Vec2f center{};
  float radius = 0.0f;

  Circle() = default;
  Circle(const Vec2f &center, float radius) : center(center), radius(radius) {}
};

struct HitResult {
  Vec2f delta{};
  bool hit = false;

  HitResult() = default;
  HitResult(bool hit, const Vec2f &delta) : delta(delta), hit(hit) {}

  operator bool() const { return hit; }
};

struct Line {
  Vec2f start{}, end{};

  Line() = default;
  Line(const Vec2f &start, const Vec2f &end) : start(start), end(end) {}

  float length2() const { return (end - start).length2(); }
  float length() const { return (end - start).length(); }

  Vec2f dir() const { return end - start; }
  Vec2f ndir() const { return (end - start).normalized(); }

  Vec2f normal() const { return (end - start).normalized().ort(); }

  friend bool intersects(const Line &alpha, const Line &beta);
  friend float dist(const Line &line, const Vec2f &point);
  friend HitResult hit(const Line &line, const Circle &circle);
};

HitResult hit(const Line &line, const Circle &circle);
