#include <iostream>

#include <cmath>
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"

/***
 * A top-down care racer from this video::
 *   https://www.youtube.com/watch?v=YzhhVHb0WVY
 */

/// check out reynold's steering behaviours

const int num = 8;  // checkpoints
int points[num][2] = {{300, 610}, {1270, 430}, {1380, 2380}, {1900, 2460}, {1970, 1700}, {2550, 1680}, {2560, 3150}, {500, 3300}};

struct Car {
  float x, y, speed, angle;
  int n;

  Car() {
    speed = 2;
    angle = 0;
    n = 0;
  }

  void move() {
    x += sin(angle) * speed;
    y -= cos(angle) * speed;
  }

  void findTarget() {
    float tx = points[n][0];
    float ty = points[n][1];
    float beta = angle - atan2(tx - x, -ty + y);
    if (sin(beta) < 0) {
      angle += 0.025 * speed;
    } else {
      angle -= 0.025 * speed;
    }
    float distance_squared_to_target = (x - tx) * (x - tx) + (y - ty) * (y - ty);
    float min_distance_squared = 5 * 5;
    if (distance_squared_to_target < min_distance_squared)
      n = (n + 1) % num;
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

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(1920, 1080), WINDOW_TITLE, sf::Style::Default, settings};
  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);

  sf::Texture bg_texture;
  bg_texture.loadFromFile("assets/images/background.png");  // 1440x1824
  sf::Sprite sBackground(bg_texture);

  sf::Vector2u texture_size = bg_texture.getSize();
  sf::Vector2u window_size = window.getSize();
  sf::FloatRect window_rect(0, 0, window_size.x, window_size.y);

  // Initial view setup
  float zoom_factor = 1.0f;
  updateView(window, bg_texture);

  sf::CircleShape centre(20);
  centre.setOrigin(10, 10);
  centre.setFillColor(sf::Color::Blue);
  centre.setPosition((float)texture_size.x / 2, (float)texture_size.y / 2);

  sf::Clock deltaClock{};  /// Keeps track of elapsed time
                           ///////////////////////////////////////////////////////////////////////////////////////////////////

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      /// This is how you listen for the window close button being pressed
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::Resized) {
        // Update the view on window resize
        updateView(window, bg_texture, zoom_factor);
      } else if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
          zoom_factor *= 1.1f;
        } else if (event.mouseWheelScroll.delta < 0) {
          zoom_factor /= 1.1f;
        }
        updateView(window, bg_texture, zoom_factor);
      } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Right) {
          zoom_factor = 1.0;
          updateView(window, bg_texture, zoom_factor);
        }
      }

      //      // Zoom view with mouse wheel
      //      if (event.type == sf::Event::MouseWheelScrolled) {
      //        if (event.mouseWheelScroll.delta > 0) {
      //          view.zoom(15.0f / 16.0f);  // Zoom in
      //        } else {
      //          view.zoom(16.0 / 15.0);  // Zoom out
      //        }
      //        window.setView(view);
      //      }
    }
    //
    sf::Time time = deltaClock.restart();
    ////////////////////////////////////////////////////////////////////////////////////////////////
    window.clear();
    //    sBackground.setPosition(-offsetX, -offsetY);
    window.draw(sBackground);

    window.draw(centre);

    window.display();
  }

  return 0;
}
