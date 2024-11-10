#include <iostream>

#include <algorithm>
#include <cmath>
#include "../501a-noc-vectors/pvector.h"
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"

/***
 * A top-down care racer from this video::
 *   https://www.youtube.com/watch?v=YzhhVHb0WVY
 */

/// check out reynold's steering behaviours

const int waypoint_count = 9;  // checkpoints
int waypoints[waypoint_count][2] = {{186, 292}, {600, 226}, {690, 1220}, {920, 1270}, {950, 900}, {1200, 810}, {1250, 1500}, {1100, 1640}, {220, 1620}};
struct Vehicle {
  float x;
  float y;
  float theta = 0;
  float speed;
  float top_speed;
  //  float angle;
  int n;
  float v_max = 360.0f;
  PVector m_position;
  PVector m_velocity;
  PVector m_acceleration;

  Vehicle() {
    speed = 1;
    theta = 0;
    n = 0;
    m_velocity = PVector(1, 0);
  }

  void apply_force(PVector f) { m_acceleration += f; }

  void update(float delta_t) {
    m_velocity += m_acceleration * delta_t;
    m_velocity.limit(v_max);
    m_position += m_velocity * delta_t;
    m_acceleration = PVector(0, 0);
  }

  void move() {
    x += sin(theta) * speed;
    y -= cos(theta) * speed;
  }

  void findTarget() {
    float tx = waypoints[n][0];
    float ty = waypoints[n][1];
    float beta = theta - atan2(tx - x, -ty + y);
    if (sin(beta) < 0) {
      theta += 0.010 * speed;
    } else {
      theta -= 0.010 * speed;
    }
    float distance_squared_to_target = (x - tx) * (x - tx) + (y - ty) * (y - ty);
    float min_distance_squared = 5 * 5;
    if (distance_squared_to_target < min_distance_squared) {
      n = (n + 1) % waypoint_count;
    }
    if (distance_squared_to_target < 100) {
      speed = top_speed / 10;
    } else {
      speed = top_speed;
    }
    if (speed < 2.0) {
      speed = 2.0;
    }
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(1920, 1080), WINDOW_TITLE, sf::Style::Default, settings};
  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);

  sf::Texture bg_texture;
  bg_texture.loadFromFile("assets/images/course.png");  // 1440x1824
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

  sf::CircleShape waypoint_marker(20);
  waypoint_marker.setOrigin(10, 10);
  waypoint_marker.setFillColor(sf::Color::Green);
  centre.setPosition((float)texture_size.x / 2, (float)texture_size.y / 2);

  sf::Texture car_texture;
  car_texture.loadFromFile("assets/images/car.png");
  car_texture.setSmooth(true);

  sf::Sprite sCar(car_texture);
  sCar.setOrigin(22, 22);
  float R = 22;
  const int car_count = 5;
  Vehicle car[car_count];
  for (int i = 0; i < car_count; i++) {
    car[i].x = 150;  //+ i * 50;
    car[i].y = 1200 - i * 10;
    car[i].top_speed = 4 + i;
  }

  Vehicle mini;
  mini.m_position = PVector(150, 1300);
  mini.m_velocity = PVector(0, 0);
  mini.m_acceleration = PVector(20, -200);

  sf::Color Colors[10] = {sf::Color::Red, sf::Color::Green, sf::Color::Magenta, sf::Color::Blue, sf::Color::White};

  sf::Clock deltaClock{};  /// Keeps track of elapsed time
                           ///////////////////////////////////////////////////////////////////////////////////////////////////

  while (window.isOpen()) {
    sf::Time time = deltaClock.restart();
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
    ////////////////////////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < car_count; i++) {
      car[i].move();
      car[i].findTarget();
    }

    // collision
    for (int i = 0; i < car_count; i++)
      for (int j = 0; j < car_count; j++) {
        int dx = 0, dy = 0;
        while (dx * dx + dy * dy < 4 * R * R) {
          car[i].x += dx / 10.0;
          car[i].x += dy / 10.0;
          car[j].x -= dx / 10.0;
          car[j].y -= dy / 10.0;
          dx = car[i].x - car[j].x;
          dy = car[i].y - car[j].y;
          if (!dx && !dy)
            break;
        }
      }

    sf::CircleShape blob(20);
    blob.setOrigin(10, 10);
    blob.setFillColor(sf::Color::Red);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    window.clear();
    //    sBackground.setPosition(-offsetX, -offsetY);
    window.draw(sBackground);

    int offsetX = 0;
    int offsetY = 0;
    for (int i = 0; i < car_count; i++) {
      sCar.setPosition(car[i].x - offsetX, car[i].y - offsetY);
      sCar.setRotation(car[i].theta * 180 / 3.141593);
      sCar.setColor(Colors[i]);
      window.draw(sCar);
    }
    PVector force(10, 0);
    //    force.rotate_to(mini.m_velocity.angle() + M_PI / 2);
    mini.apply_force(force);
    force.rotate_by(M_PI / 2);
    force /= 2;
    mini.apply_force(force);
    mini.update(time.asSeconds());
    blob.setPosition(mini.m_position.x, mini.m_position.y);
    window.draw(blob);

    /// draw markers for debugging
    for (int i = 0; i < waypoint_count; i++) {
      waypoint_marker.setPosition(waypoints[i][0], waypoints[i][1]);
      window.draw(waypoint_marker);
    }
    //    window.draw(centre);
    window.display();
  }

  return 0;
}
