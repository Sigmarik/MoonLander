#include "Geometry.h"

Vec2f operator+(const Vec2f &alpha, const Vec2f &beta) {
  return Vec2f(alpha.x + beta.x, alpha.y + beta.y);
}

Vec2f operator-(const Vec2f &alpha, const Vec2f &beta) {
  return Vec2f(alpha.x - beta.x, alpha.y - beta.y);
}

Vec2f operator*(const Vec2f &alpha, const Vec2f &beta) {
  return Vec2f(alpha.x * beta.x, alpha.y * beta.y);
}

Vec2f operator/(const Vec2f &alpha, const Vec2f &beta) {
  return Vec2f(alpha.x / beta.x, alpha.y / beta.y);
}

Vec2f operator*(const Vec2f &alpha, float value) {
  return Vec2f(alpha.x * value, alpha.y * value);
}

Vec2f operator/(const Vec2f &alpha, float value) {
  return Vec2f(alpha.x / value, alpha.y / value);
}

float dot(const Vec2f &alpha, const Vec2f &beta) {
  return alpha.x * beta.x + alpha.y * beta.y;
}

float cross(const Vec2f &alpha, const Vec2f &beta) {
  return alpha.x * beta.y - alpha.y * beta.x;
}

float dist(const Vec2f &alpha, const Vec2f &beta) {
  return (alpha - beta).length();
}

float dist2(const Vec2f &alpha, const Vec2f &beta) {
  return (alpha - beta).length2();
}

bool intersects(const Line &alpha, const Line &beta) {
  float sign_left = cross(alpha.start - beta.start, beta.end - beta.start) *
                    cross(beta.end - beta.start, alpha.end - beta.start);
  float sign_right = cross(beta.start - alpha.start, alpha.end - alpha.start) *
                     cross(alpha.end - alpha.start, beta.end - alpha.start);
  return sign_left >= 0.0 && sign_right >= 0.0;
}

float dist(const Line &line, const Vec2f &point) {
  float dot_prod = dot(point - line.start, line.dir());

  if (dot_prod < 0.0f || dot_prod > line.length2()) {
    return std::min(dist(point, line.start), dist(point, line.end));
  }

  Vec2f projection = line.dir() * dot_prod / line.length2();

  return dist(projection, point - line.start);
}

HitResult hit(const Line &line, const Circle &circle) {
  HitResult hit{};

  float dot_prod = dot(circle.center - line.start, line.dir());

  if (dot_prod <= 0.0f || dot_prod > line.length2()) {
    float dist_start = dist(circle.center, line.start);
    float dist_end = dist(circle.center, line.end);

    Vec2f point_of_interest = line.start;
    float distance_of_interest = dist_start;

    if (dist_end <= dist_start) {
      point_of_interest = line.end;
      distance_of_interest = dist_end;
    }

    if (distance_of_interest <= circle.radius) {
      hit.delta = (circle.center - point_of_interest).normalized() *
                  (circle.radius - distance_of_interest);
      hit.hit = true;
    }

    return hit;
  }

  Vec2f projection = line.start + line.dir() * dot_prod / line.length2();
  Vec2f vertical = circle.center - projection;
  float distance = vertical.length();

  if (distance <= circle.radius) {
    hit.delta = vertical.normalized() * (circle.radius - distance);
    hit.hit = true;
  }

  return hit;
}
