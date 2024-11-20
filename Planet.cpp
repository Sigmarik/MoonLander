#include "Planet.h"

#include "Rendering.h"
#include <assert.h>

Planet::Planet(const Vec2f &center, float radius, const Noise &noise,
               size_t segments)
    : center_(center), radius_(radius), surface_(segments, Line()) {
  assert(segments > 6);

  Vec2f rv = Vec2f::fromAzimuth(0.0f, radius);
  Vec2f prev_point = rv * (1.0 + noise(rv));

  for (size_t id = 0; id < segments; ++id) {
    rv = Vec2f::fromAzimuth((id + 1) * 2.0f * M_PI / segments, radius);
    Vec2f new_point = rv * (1.0 + noise(rv));

    Line &segment = surface_[id];
    segment.start = prev_point;
    segment.end = new_point;

    prev_point = new_point;
  }
}

HitResult Planet::hit(const Circle &circle) const {
  HitResult result{};

  Vec2f relative_center = circle.center - center_;
  Vec2f fixed_center = fixHeight(relative_center, circle.radius / 2);

  result.delta = fixed_center - relative_center;

  Circle relative_circle{};
  relative_circle.radius = circle.radius;
  relative_circle.center = fixed_center;

  size_t segment_id = findSegment(relative_circle.center);
  size_t first_segment = segment_id == 0 ? surface_.size() - 1 : segment_id - 1;

  for (size_t seg_id = first_segment;
       seg_id != (segment_id + 2) % surface_.size();
       seg_id = (seg_id + 1) % surface_.size()) {
    HitResult segment_hit = ::hit(surface_[seg_id], relative_circle);

    if (segment_hit) {
      result.hit = true;
      result.delta = result.delta + segment_hit.delta;
      relative_circle.center = relative_circle.center + segment_hit.delta;
    }
  }

  return result;
}

size_t Planet::findSegment(const Vec2f &point) const {
  Vec2f rad_vector = point - center_;
  float azimuth = rad_vector.azimuth();

  float segment_length = (2 * M_PI) / surface_.size();

  return std::floor(azimuth / segment_length);
}

float Planet::heightAtPoint(const Vec2f &point) const {
  Vec2f rad_vector = point - center_;
  float azimuth = rad_vector.azimuth();

  float segment_length = (2 * M_PI) / surface_.size();

  size_t seg_id = std::floor(azimuth / segment_length);

  float partial = azimuth / segment_length - seg_id;

  Line segment = surface_[seg_id];
  float avg = segment.end.length() * partial +
              segment.start.length() * (1.0f - partial);

  return avg;
}

Vec2f Planet::fixHeight(const Vec2f &relative_point, float addition) const {
  float height = relative_point.length();
  float desired_height = heightAtPoint(relative_point) + addition;

  if (height >= desired_height) {
    return relative_point;
  }
  return relative_point * desired_height / height;
}

void Planet::draw(Screen &screen) {
  IntColor color = IntColor(157, 158, 173);
  IntColor markings_color = color * 0.5;
  IntColor coreline_color = markings_color * 0.5;

  for (size_t id = 0; id < surface_.size(); ++id) {
    const Line &segment = surface_[id];
    Line absolute_segment(segment.start + center_, segment.end + center_);

    if (id % 5 == 0) {
      screen.drawLine(Line(absolute_segment.end, center_), coreline_color);
    } else {
      Vec2f point = absolute_segment.end;
      screen.drawLine(Line(point, point + (center_ - point).normalized() * 0.1),
                      markings_color);
    }

    screen.drawLine(absolute_segment, color);
  }
}

Vec2f Planet::gravity(const Vec2f &position) const {
  Vec2f rad_vec = center_ - position;

  float mass = radius_ * radius_ * radius_ * density_;

  float pull = mass / rad_vec.length2();

  return rad_vec.normalized() * pull;
}

float Noise::operator()(const Vec2f &point) const {
  float mountains = cos(point.y * 3.0 * scale_) * amplitude_;
  float hills = cos((point.x + point.y) * 10 * scale_) * amplitude_ / 7;
  float craters =
      pow(sin((point.x - point.y) * 20.0 * scale_), 10) * amplitude_ / 20;

  return mountains + hills + craters;
}
