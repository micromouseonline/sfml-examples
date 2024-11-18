#include <math.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "expfilter.h"
#include "robot.h"
#include "robotview.h"
#include "vec2.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Returns distance between points
inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
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
  sf::IntRect blocked(5 * 76, 0, 76, 100);
  mouse.setTextureRect(normal);
  mouse.setOrigin(38, 60);
  mouse.setPosition(286, 466);

  /// lets create an image of one frame of the mouse
  sf::IntRect r(0, 0, 76, 100);
  sf::Image the_mouse;
  sf::Color bg(16, 16, 16, 0);
  the_mouse.create(r.width, r.height, bg);
  the_mouse.copy(mouse_images, 0, 0, r);

  sf::Text text;
  text.setString("Here is some text");
  text.setFont(font);
  text.setCharacterSize(24);                     // in pixels, not points!
  text.setFillColor(sf::Color(255, 0, 0, 255));  // it can be any colour//  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles

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
    sf::Clock clock;
    sf::Color shield_colour = sf::Color::Green;

    // Clear the window
    window.clear(sf::Color::Black);
    //    window.draw(mouse);
    int phase1 = clock.restart().asMicroseconds();

    int phase2 = clock.restart().asMicroseconds();

    float r = 1;
    sf::CircleShape circle(r);
    circle.setOrigin(r, r);
    circle.setFillColor(shield_colour);
    int phase3 = clock.restart().asMicroseconds();

    std::string string = "";
    string = "times:\n";
    string += "test and move: " + std::to_string(phase1) + " us\n";
    string += "get mouse pos: " + std::to_string(phase2) + " us\n";
    string += "  draw shield: " + std::to_string(phase3) + " us\n";
    int total = phase1 + phase2 + phase3;
    text.setString(string);
    text.setPosition(900, 50);
    window.draw(text);
    // Display the window
    window.display();
  }

  return 0;
}
