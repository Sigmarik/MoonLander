#include "Engine.h"
#include <memory.h>
#include <stdlib.h>

#include <iostream>
#include <stdio.h>

#include "Planet.h"
#include "Rendering.h"
#include "Spaceship.h"

//! There are now words to describe how much I hate global variables...
//! But, since the screen buffer is a global variable, and there are no
//! persistent structured defined that would have transferred anything between
//! game stages, I have to create a singleton that has no valid reason to
//! be one.

struct Game {
  static Screen screen;
  static float time;

  static Planet planet;
  static Spaceship spaceship;
};

//! Static initialization, everybody!
//! SIO-mismanaged assemblers, get screwed!

Screen Game::screen = Screen(&buffer[0][0], SCREEN_WIDTH, SCREEN_HEIGHT);
float Game::time = 0.0f;
Planet Game::planet(Vec2f(0.0, 0.0), 100.0, Noise(0.01, 0.3), 10000);
Spaceship Game::spaceship{};

void initialize() {
  Game::spaceship =
      Spaceship(Vec2f(0.0, Game::planet.heightAtPoint(Vec2f(0, 1)) + 1.0));
}

static void updateZoom(float dt) {
  float zoom_change = 0.0;

  if (is_key_pressed(VK_UP)) {
    zoom_change += 1;
  }

  if (is_key_pressed(VK_DOWN)) {
    zoom_change -= 1;
  }

  Game::screen.setScale(std::max(
      Game::screen.getScale() * (1.0 + zoom_change * 0.5 * dt), 0.0001));
}

void act(float dt) {
  Game::time += dt;

  Game::spaceship.update(Game::planet, dt);
  Game::screen.setShift(Game::spaceship.getPosition());

  updateZoom(dt);

  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();
}

void draw() {
  Game::screen.flush();
  Game::spaceship.draw(Game::screen);
  Game::planet.draw(Game::screen);
}

void finalize() { std::cout << "Have a nice day!" << std::endl; }
