#include "Spaceship.h"
#include "Engine.h"
#include "Rendering.h"

void Spaceship::update(const Planet &planet, float delta_time) {
  //* Note: Screw it. I am not implementing scene class with generalized
  //* component access again. If you want to see how I do this, look no
  //* further than my GD framework: https://github.com/Sigmarik/graphics-engine
  //* For now, though, I will have to suffice myself with the old fashion way:
  //* explicit component references.

  Vec2f gravity = planet.gravity(position_);

  position_ += velocity_ * delta_time;

  HitResult hit = planet.hit(Circle(position_, radius_));

  if (hit) {
    surfaced_ = true;
    last_impact_ = hit.delta;

    processCollision(hit, delta_time);
  } else {
    surfaced_ = false;
  }

  is_accelerating_ = is_key_pressed(VK_SPACE);

  //* I would have implemented remapable input...
  //* if I did not do it in the framework.
  Vec2f thrust =
      is_accelerating_ ? Vec2f::fromAzimuth(rotation_, thrust_) : Vec2f{};
  Vec2f acceleration = gravity + thrust;

  velocity_ += acceleration * delta_time;

  updateRotation(delta_time);
}

void Spaceship::draw(Screen &screen) {
  IntColor body_color(0, 255, 0);
  IntColor shield_color(60, 60, 70);

  unsigned shield_detail = 16;
  for (unsigned id = 0; id < shield_detail; ++id) {
    Vec2f start =
        Vec2f::fromAzimuth((float)id / shield_detail * 2 * M_PI, radius_);
    Vec2f end =
        Vec2f::fromAzimuth((float)(id + 1) / shield_detail * 2 * M_PI, radius_);

    screen.drawLine(Line(start + position_, end + position_), shield_color);
  }

  Vec2f nose = Vec2f::fromAzimuth(rotation_, radius_) + position_;
  Vec2f left_wing =
      Vec2f::fromAzimuth(rotation_ + M_PI * 2 / 3, radius_ / 2) + position_;
  Vec2f right_wing =
      Vec2f::fromAzimuth(rotation_ + M_PI * 4 / 3, radius_ / 2) + position_;

  Vec2f back =
      Vec2f::fromAzimuth(rotation_ + M_PI, radius_ * 2 / 3) + position_;

  screen.drawLine(Line(nose, left_wing), body_color);
  screen.drawLine(Line(nose, right_wing), body_color);
  screen.drawLine(Line(right_wing, left_wing), body_color);

  if (is_accelerating_) {
    IntColor flame_colors[] = {
        IntColor(255, 10, 10),
        IntColor(255, 255, 10),
    };
    unsigned stage_count = sizeof(flame_colors) / sizeof(*flame_colors);

    screen.drawLine(Line(right_wing, back),
                    flame_colors[flame_flicker_stage_ % stage_count]);
    screen.drawLine(Line(left_wing, back),
                    flame_colors[flame_flicker_stage_ % stage_count]);

    flame_flicker_stage_++;
  }
}

void Spaceship::processCollision(const HitResult &hit, float delta_time) {
  position_ += hit.delta;

  if (dot(hit.delta, velocity_) < 0) {
    Vec2f vertical =
        hit.delta * (dot(hit.delta, velocity_) / hit.delta.length2());
    Vec2f horizontal = velocity_ - vertical;

    vertical = -vertical * bounciness_;

    float friction_decrease = friction_ * delta_time;
    if (horizontal.length() < friction_decrease) {
      horizontal = Vec2f(0.0);
    } else {
      horizontal =
          horizontal.normalized() * (horizontal.length() - friction_decrease);
    }

    velocity_ = vertical + horizontal;
  }
}

void Spaceship::updateRotation(float delta_time) {
  float delta_rotation = 0.0f;

  if (is_key_pressed(VK_LEFT)) {
    delta_rotation += 1.0;
  }
  if (is_key_pressed(VK_RIGHT)) {
    delta_rotation -= 1.0;
  }

  rotation_ += delta_rotation * maneuverability_ * delta_time;
}
