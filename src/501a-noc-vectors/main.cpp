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

struct Mover {
  float v_max = 360.0f;
  PVector m_position;
  PVector m_velocity;
  PVector m_acceleration;

  Mover(float x, float y) { m_position = {x, y}; }

  void apply_force(PVector f) { m_acceleration += f; }

  void update(float delta_t) {
    m_velocity += m_acceleration * delta_t;
    m_velocity.limit(v_max);
    m_position += m_velocity * delta_t;
    if (m_position.x > WINDOW_WIDTH) {
      m_velocity.x *= -1;
    }
    if (m_position.x < 0) {
      m_velocity.x *= -1;
    }
    if (m_position.y > WINDOW_HEIGHT) {
      m_velocity.y *= -1;
    }
    if (m_position.y < 0) {
      m_velocity.y *= -1;
    }

    m_acceleration = {0, 0};
  }

  void draw(sf::RenderTarget& target) {
    float r = 10.0f;
    sf::CircleShape blob(r);
    blob.setOrigin(r / 2, r / 2);
    blob.setFillColor(sf::Color::Red);
    blob.setPosition(m_position.x, m_position.y);
    target.draw(blob);
  }
};

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

void vector_test() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isMouseInsideWindow(const sf::RenderWindow& window) {      // Get the current mouse position relative to the window
  sf::Vector2i mousePosition = sf::Mouse::getPosition(window);  // Get the window size
  sf::Vector2u windowSize = window.getSize();                   // Check if the mouse position is within the window bounds
  return (mousePosition.x >= 0 && mousePosition.x < static_cast<int>(windowSize.x) && mousePosition.y >= 0 && mousePosition.y < static_cast<int>(windowSize.y));
}

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Default, settings};

  window.setFramerateLimit(60);
  //  window.setVerticalSyncEnabled(true);

  sf::Clock deltaClock{};  /// Keeps track of elapsed time
  Mover mover((float)window.getSize().x / 2, (float)window.getSize().y / 2);
  mover.m_velocity = {150, -50};

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
      dir = mp - mover.m_position;
      dir.normalize();
      dir *= 220;
    }
    mover.apply_force(dir);
    mover.update(time.asSeconds());

    window.clear();
    mover.draw(window);

    window.display();
  }

  return 0;
}
