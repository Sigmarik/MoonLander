#pragma once

#include "Geometry.h"
#include "Planet.h"

struct Screen;

struct Spaceship {
  Spaceship(const Vec2f &position = Vec2f(), const Vec2f &velocity = Vec2f())
      : position_(position), velocity_(velocity) {};

  void update(const Planet &planet, float delta_time);

  Vec2f getPosition() const { return position_; }

  void draw(Screen &screen);

private:
  void drawShield(Screen &screen);
  void drawBody(Screen &screen);
  void drawArrows(Screen &screen);

  void processCollision(const HitResult &hit, float delta_time);
  void updateRotation(float delta_time);

  Vec2f position_;
  Vec2f velocity_;

  float radius_ = 0.1;
  float rotation_ = 0.0;

  float thrust_ = 1.0;

  bool surfaced_ = false;
  Vec2f last_impact_{};

  float bounciness_ = 0.2;
  float friction_ = 1.0;

  float maneuverability_ = 5;

  float time_since_hit_ = 1e10;
  float toughness_ = 0.1;

  unsigned flickering_stage_ = 0;
  bool is_accelerating_ = false;
};
