#include <iostream>

#include <algorithm>
#include <cmath>
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
#include "pvector.h"

/// check out reynold's steering behaviours

/// https://natureofcode.com/vectors

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;

#include "vehicle.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateView(sf::RenderWindow& window, sf::Texture& bg_texture, float zoom_factor = 1.0) {
  sf::Vector2u texture_size = bg_texture.getSize();
  sf::Vector2u window_size = window.getSize();
  sf::FloatRect window_rect(0, 0, window_size.x, window_size.y);

  sf::View view(window_rect);
  view.setCenter((float)texture_size.x / 2, (float)texture_size.y / 2);

  // Calculate the scale factor to maintain aspect ratio and fill the window vertically
  float scale_y = (float)texture_size.y / (float)window_size.y;
  view.zoom(scale_y * zoom_factor);
  window.setView(view);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isMouseInsideWindow(const sf::RenderWindow& window) {      // Get the current mouse position relative to the window
  sf::Vector2i mousePosition = sf::Mouse::getPosition(window);  // Get the window size
  sf::Vector2u windowSize = window.getSize();                   // Check if the mouse position is within the window bounds
  return (mousePosition.x >= 0 && mousePosition.x < static_cast<int>(windowSize.x) && mousePosition.y >= 0 && mousePosition.y < static_cast<int>(windowSize.y));
}

void draw_vector(sf::RenderTarget& target, PVector pos, PVector vec, sf::Color color = sf::Color::White) {
  sf::Vertex line[2];
  line[0].position = {pos.x, pos.y};
  line[0].color = color;
  vec += pos;
  line[1].position = {vec.x, vec.y};
  line[1].color = color;
  target.draw(line, 2, sf::Lines);
}

void draw_line(sf::RenderTarget& target, PVector pos, PVector vec, sf::Color color = sf::Color::White) {
  sf::Vertex line[2];
  line[0].position = {pos.x, pos.y};
  line[0].color = color;
  line[1].position = {vec.x, vec.y};
  line[1].color = color;
  target.draw(line, 2, sf::Lines);
}

enum {
  DRAW_NONE = 0,
  DRAW_VELOCITY = 1 << 0,
  DRAW_FORCE = 1 << 2,
  DRAW_DESIRED = 1 << 3,
};

void draw_vehicle(sf::RenderTarget& canvas, Vehicle& vehicle, int flags, sf::Color color = sf::Color::White, sf::Sprite* sprite = nullptr) {
  sf::CircleShape blob(6);
  blob.setOrigin(3, 3);
  blob.setFillColor(color);
  PVector pos = vehicle.m_position;
  PVector vel = vehicle.m_velocity;
  PVector force = vehicle.m_force;
  PVector desired = vehicle.m_desired;

  blob.setPosition(pos.x, pos.y);
  canvas.draw(blob);
  if (sprite) {
    sprite->setPosition(pos.x, pos.y);
    sprite->setRotation(vel.angle() * 57.29 + 90);
    canvas.draw(*sprite);
  }
  if (flags == DRAW_NONE) {
    return;
  }
  if (flags & DRAW_VELOCITY) {
    draw_vector(canvas, pos, vel, sf::Color::Blue);
  }
  if (flags & DRAW_FORCE) {
    draw_vector(canvas, pos, force, sf::Color::Yellow);
  }
  if (flags & DRAW_DESIRED) {
    draw_vector(canvas, pos, desired, sf::Color::Green);
  }
}

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Default, settings};

  window.setFramerateLimit(60);
  //  window.setVerticalSyncEnabled(true);

  sf::Clock deltaClock{};  /// Keeps track of elapsed time
  Vehicle mover((float)window.getSize().x / 2, (float)window.getSize().y / 2);
  //  mover.m_velocity = {150, -50};

  while (window.isOpen()) {
    sf::Time time = deltaClock.restart();
    sf::Event event{};
    while (window.pollEvent(event)) {
      /// This is how you listen for the window close button being pressed
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }

    PVector dir(0, 0);
    if (isMouseInsideWindow(window)) {
      sf::Vector2 mousePosition = sf::Mouse::getPosition(window);
      PVector mp(mousePosition.x, mousePosition.y);
      mover.arrive(mp);
    }
    //    mover.apply_force(dir);
    mover.update(time.asSeconds());

    window.clear();

    draw_vehicle(window, mover, DRAW_VELOCITY);
    window.display();
  }

  return 0;
}
