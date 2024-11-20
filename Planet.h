#pragma once

#include <stdlib.h>
#include <vector>

#include "Geometry.h"

struct Noise {
  Noise() = default;
  Noise(float amplitude, float scale = 1.0)
      : amplitude_(amplitude), scale_(scale) {}

  float operator()(const Vec2f &point) const;

private:
  float amplitude_ = 1.0;
  float scale_ = 1.0;
};

struct Screen;

struct Planet {
  Planet(const Vec2f &center, float radius,
         const Noise &noise = Noise(0.1, 1.0), size_t segments = 1024);

  HitResult hit(const Circle &circle) const;

  void draw(Screen &screen);

  Vec2f gravity(const Vec2f &position) const;

  float getRadius() const { return radius_; }
  float heightAtPoint(const Vec2f &point) const;

private:
  size_t findSegment(const Vec2f &point) const;
  Vec2f fixHeight(const Vec2f &relative_point, float addition = 0.0f) const;

  Vec2f center_{};
  float radius_ = 1.0f;

  float density_ = 0.001f;

  std::vector<Line> surface_{};
};
