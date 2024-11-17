#include <math.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "expfilter.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Returns distance between points
inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

bool isWithinBounds(const sf::Vector2u& size, int x, int y) {
  return x >= 0 && x < static_cast<int>(size.x) && y >= 0 && y < static_cast<int>(size.y);
}

sf::Color getColorAtPixel(const sf::Image& image, int x, int y) {
  return isWithinBounds(image.getSize(), x, y) ? image.getPixel(x, y) : sf::Color::Transparent;
}

sf::Color getColorAtPixel(const sf::Image& image, sf::Vector2f point) {
  int x = (int)point.x;
  int y = (int)point.y;
  return isWithinBounds(image.getSize(), x, y) ? image.getPixel(x, y) : sf::Color::Transparent;
}

/***
 * A Bresenham style raycast. There may be no performance benefit compared to a
 * linear interpolation solution.
 *
 * casts out a single ray
 *
 * This needs encapsulating in a class
 *
 * @param image - the image holding the map. think of it like an occupancy grid
 * @param origin - the start point for the ray
 * @param angle - the angle of the ray
 * @return - the distance at which it hits the wall colour or runs out of range
 */
sf::Vector2f castRay(const sf::Image& image, sf::Color wall_colour, const sf::Vector2f& origin, float angle) {
  angle = angle * (3.14159 / 180);  // Convert angle to radians
  float range = 254;                // the maximum distance we will cast out
  sf::Vector2f rayDirection(cos(angle), sin(angle));
  sf::Vector2f dest = origin + range * rayDirection;
  sf::Color color = getColorAtPixel(image, static_cast<int>(origin.x), static_cast<int>(origin.y));

  // Bresenham's line algorithm for efficient traversal - is it though?
  int x0 = static_cast<int>(origin.x);
  int y0 = static_cast<int>(origin.y);
  int x1 = static_cast<int>(dest.x);
  int y1 = static_cast<int>(dest.y);

  int dx = std::abs(x1 - x0);
  int dy = std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;

  while (x0 != x1 || y0 != y1) {
    if (!isWithinBounds(image.getSize(), x0, y0)) {
      return {(float)x0, (float)y0};
    }

    color = getColorAtPixel(image, x0, y0);
    if (color == wall_colour || color.a == 0) {
      break;
    }

    int err2 = 2 * err;
    if (err2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (err2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
  return {(float)x0, (float)y0};
}

std::vector<sf::Vector2f> base_shield_points;
void create_collision_shield(sf::Image& image, float cx, float cy, int steps) {
  base_shield_points.clear();
  base_shield_points.resize(steps);
  float radius = 60.0f;
  for (int i = 0; i < steps; i++) {
    float angle = (2 * M_PI * i) / steps;
    float x = radius * sin(angle);
    float y = radius * cos(angle);
    sf::Vector2f point = castRay(image, sf::Color::Transparent, {cx, cy}, 57.29f * angle);
    base_shield_points.emplace_back(point - sf::Vector2f(cx, cy));
  }
}

int offsx = 10;
int offsy = 10;

sf::Vector2f get_hwall_pos(int x, int y) {
  return sf::Vector2f(offsx + 13 + 180 * x, offsy + 0 + 180 * y);
}
sf::Vector2f get_vwall_pos(int x, int y) {
  return sf::Vector2f(offsx + 0 + 180 * x, offsy + 13 + 180 * y);
}
void create_obstacles(sf::RenderTarget& target) {
  sf::RectangleShape post(sf::Vector2f(12, 12));
  post.setFillColor(sf::Color::Red);
  sf::RectangleShape v_wall(sf::Vector2f(12, 166));
  v_wall.setFillColor(sf::Color::Red);
  sf::RectangleShape h_wall(sf::Vector2f(166, 12));
  h_wall.setFillColor(sf::Color::Red);

  v_wall.setPosition(120, 80 + 13);
  for (int y = 0; y < 6; y++) {
    for (int x = 0; x < 6; x++) {
      post.setPosition(x * 180 + offsx, y * 180 + offsy);
      target.draw(post);
    }
  }
  for (int i = 0; i < 5; i++) {
    v_wall.setPosition(get_vwall_pos(0, i));
    target.draw(v_wall);
    h_wall.setPosition(get_hwall_pos(i, 0));
    target.draw(h_wall);
  }
  h_wall.setPosition(get_hwall_pos(2, 3));
  target.draw(h_wall);
  h_wall.setPosition(get_hwall_pos(3, 3));
  target.draw(h_wall);
  v_wall.setPosition(get_vwall_pos(3, 3));
  target.draw(v_wall);
  h_wall.setPosition(get_hwall_pos(1, 1));
  target.draw(h_wall);
  h_wall.setPosition(get_hwall_pos(1, 2));
  target.draw(h_wall);
  h_wall.setPosition(get_hwall_pos(2, 1));
  target.draw(h_wall);
  h_wall.setPosition(get_hwall_pos(3, 2));
  target.draw(h_wall);
  h_wall.setPosition(get_hwall_pos(1, 4));
  target.draw(h_wall);
  v_wall.setPosition(get_vwall_pos(2, 2));
  target.draw(v_wall);
  v_wall.setPosition(get_vwall_pos(4, 1));
  target.draw(v_wall);
  v_wall.setPosition(get_vwall_pos(1, 3));
  target.draw(v_wall);
  v_wall.setPosition(get_vwall_pos(4, 4));
  target.draw(v_wall);
}

// Function to rotate a point around the origin by a given angle (in radians)
sf::Vector2f rotatePoint(const sf::Vector2f& point, float angleRad) {
  float s = std::sin(angleRad);
  float c = std::cos(angleRad);

  float xNew = point.x * c - point.y * s;
  float yNew = point.x * s + point.y * c;

  return sf::Vector2f(xNew, yNew);
}

// Function to rotate all points in a vector around the origin by a given angle (in degrees)
void rotatePoints(std::vector<sf::Vector2f>& points, float angleDeg) {
  // Convert angle from degrees to radians
  float angleRad = angleDeg * (3.14159f / 180.f);

  for (auto& point : points) {
    point = rotatePoint(point, angleRad);
  }
}

int main() {
  // Create the window
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window(sf::VideoMode(1200, 900), WINDOW_TITLE, sf::Style::Default, settings);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }
  // Create a RenderTexture
  sf::RenderTexture map_texture;
  if (!map_texture.create(800, 800)) {
    // Error handling
    return -1;
  }

  sf::Cursor cursor;
  cursor.loadFromSystem(sf::Cursor::Cross);
  window.setMouseCursor(cursor);

  /// and make a mouse sprite for more pretty
  sf::Texture mouse_texture;
  sf::Image mouse_images;
  mouse_images.loadFromFile("./assets/images/mouse-76x100.png");
  mouse_texture.loadFromImage(mouse_images);

  sf::Sprite mouse;
  mouse.setTexture(mouse_texture);
  sf::IntRect normal(0, 0, 76, 100);
  sf::IntRect blocked(4 * 76, 0, 76, 100);
  mouse.setTextureRect(normal);
  mouse.setOrigin(38, 60);
  mouse.setPosition(286, 466);

  /// lets create an image of one frame of the mouse
  sf::IntRect r(0, 0, 76, 100);
  sf::Image the_mouse;
  sf::Color bg(16, 16, 16, 0);
  the_mouse.create(r.width, r.height, bg);
  the_mouse.copy(mouse_images, 0, 0, r);

  /// Now we want to cast rays out from the centre until we see the background
  /// colour or fall off the edge
  /// Use this to get a list of pixel addresses surrounding the robot image
  /// That list can then be tested against the map image for collisions.
  /// Probably

  create_collision_shield(the_mouse, 38, 62, 90);
  using ShieldPoints = std::vector<sf::Vector2f>;
  ShieldPoints collision_shield = base_shield_points;

  sf::Text text;
  text.setString("Here is some text");
  text.setFont(font);
  text.setCharacterSize(24);                     // in pixels, not points!
  text.setFillColor(sf::Color(255, 0, 0, 255));  // it can be any colour//  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles

  map_texture.clear(sf::Color(32, 32, 32));
  create_obstacles(map_texture);
  map_texture.display();

  /// this is really slow - takes about 200us
  sf::Image map_image = map_texture.getTexture().copyToImage();
  // give the RenderTexture to a sprite
  sf::Sprite maze_map(map_texture.getTexture());
  ExpFilter<float> frame_time(0.99);
  sf::Clock frame_clock;
  // Main loop
  while (window.isOpen()) {
    // Event handling
    float dt = frame_clock.restart().asSeconds();
    float v = 200;
    float omega = 180;
    float d_theta = 0;
    float d_s = 0;
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    if (window.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        d_theta = -omega * dt;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        d_theta = omega * dt;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        d_s = v * dt;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        d_s = -v * dt;
      }
    }
    // Clear the window
    window.clear(sf::Color::Black);
    window.draw(maze_map);
    //    mouse.setPosition(mousePosition);
    window.draw(mouse);

    sf::Clock clock;
    sf::Color shield_colour = sf::Color::Green;
    float angle = mouse.getRotation() + d_theta;
    float dx = std::cos((angle - 90.0f) * 3.14f / 180.0f) * d_s;
    float dy = std::sin((angle - 90.0f) * 3.14f / 180.0f) * d_s;
    sf::Vector2f movement(dx, dy);
    /// get the entire new position, including rotation.
    /// test that for collision and then set the flag
    /// all we need do is transform the collision shield, not the robot
    ShieldPoints temp_shield = collision_shield;
    for (auto& point : temp_shield) {
      point += mouse.getPosition() + movement;
    }
    bool can_move = true;
    for (auto& point : temp_shield) {
      if (getColorAtPixel(map_image, point) == sf::Color::Red) {
        can_move = false;
        break;
      }
    }
    if (mouse.getPosition().x + dx < 1024 && mouse.getPosition().x + dx > 0) {
      if (can_move) {
        mouse.setTextureRect(normal);
        mouse.rotate(d_theta);
        mouse.move(movement);
        shield_colour = sf::Color::Transparent;
      } else {
        mouse.setTextureRect(blocked);
        shield_colour = sf::Color::Red;
        /// we can get stuck here.
        /// TODO:: get unstuck!
      }
    }
    int phase1 = clock.restart().asMicroseconds();

    sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

    int phase2 = clock.restart().asMicroseconds();

    /// takes about 1.2us per point
    collision_shield = base_shield_points;
    rotatePoints(collision_shield, angle);
    for (auto& point : collision_shield) {
      float r = 1;
      sf::CircleShape circle(r);
      circle.setOrigin(r, r);
      circle.setFillColor(shield_colour);
      circle.setPosition(mouse.getPosition() + point);
      window.draw(circle);
    }

    int phase3 = clock.restart().asMicroseconds();

    std::string string = "";
    string = "times:\n";
    string += "test and move: " + std::to_string(phase1) + " us\n";
    string += "get mouse pos: " + std::to_string(phase2) + " us\n";
    string += "rotate shield: " + std::to_string(phase3) + " us\n";
    int total = phase1 + phase2 + phase3;
    frame_time.update(total);
    string += "   total time: " + std::to_string(int(frame_time.value)) + " us\n";
    string += "Mouse: " + std::to_string((int)mousePosition.x) + ", " + std::to_string((int)mousePosition.y) + "\n";
    text.setString(string);
    text.setPosition(900, 50);
    window.draw(text);
    // Display the window
    window.display();
  }

  return 0;
}
