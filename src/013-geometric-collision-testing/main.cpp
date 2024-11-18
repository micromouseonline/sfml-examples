#include <math.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "collisions.h"
#include "expfilter.h"
#include "object.h"
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

void draw_vector(sf::RenderTarget& target, Vec2 pos, Vec2 vec, sf::Color color = sf::Color::White) {
  sf::Vertex line[2];
  line[0].position = {pos.x, pos.y};
  line[0].color = color;
  vec += pos;
  line[1].position = {vec.x, vec.y};
  line[1].color = color;
  target.draw(line, 2, sf::Lines);
}

void draw_line(sf::RenderTarget& target, Vec2 p1, Vec2 p2, sf::Color color = sf::Color::Yellow) {
  sf::Vertex line[2];
  line[0].position = {p1.x, p1.y};
  line[0].color = color;
  line[1].position = {p2.x, p2.y};
  line[1].color = color;
  target.draw(line, 2, sf::Lines);
}

void draw_point(sf::RenderTarget& target, Vec2 p, sf::Color color = sf::Color::Red) {
  float r = 2.0f;
  sf::CircleShape circle(r);
  circle.setFillColor(color);
  circle.setPosition(p.x, p.y);
  circle.setOrigin(r, r);
  target.draw(circle);
}

/////////////////////////////////////////////////////////////////////
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

std::vector<sf::RectangleShape> rectangles;
void make_rectangles() {
  for (int i = 0; i < 10; ++i) {
    sf::RectangleShape rect(sf::Vector2f(20.f, 20.f));

    // Randomly position the rectangle within the window bounds
    float x = static_cast<float>(std::rand() % (1200 - 20));
    float y = static_cast<float>(std::rand() % (900 - 20));
    rect.setPosition(x, y);

    // Optionally set a random color for each rectangle
    rect.setFillColor(
        sf::Color(static_cast<sf::Uint8>(std::rand() % 256), static_cast<sf::Uint8>(std::rand() % 256), static_cast<sf::Uint8>(std::rand() % 256)));

    rectangles.push_back(rect);
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

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(24);                     // in pixels, not points!
  text.setFillColor(sf::Color(255, 0, 0, 255));  // it can be any colour//  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles

  Vec2 a(100, 100);
  Vec2 b(100, 200);
  Vec2 p(20, 30);
  sf::CircleShape circle(30);
  circle.setOrigin(30, 30);
  circle.setFillColor(sf::Color::Blue);

  sf::RectangleShape rect(sf::Vector2f(100, 100));
  rect.setFillColor(sf::Color::Green);
  rect.setPosition(700, 200);

  sf::RectangleShape object_rect(sf::Vector2f(20, 20));
  object_rect.setOrigin(10, 10);
  object_rect.setFillColor(sf::Color::Green);
  object_rect.setPosition(200, 200);

  ComplexObject object(sf::Vector2f(0, 0));

  auto body = std::make_unique<sf::RectangleShape>(sf::Vector2f(76, 62));
  body->setFillColor(sf::Color::Transparent);
  body->setOutlineColor(sf::Color::Magenta);
  body->setOutlineThickness(2);
  body->setOrigin(38, 31);
  object.addShape(std::move(body), sf::Vector2f(0, 0));

  auto head = std::make_unique<sf::CircleShape>(38);
  head->setOrigin(38, 38);
  head->setFillColor(sf::Color::Transparent);
  head->setOutlineColor(sf::Color::Yellow);
  head->setOutlineThickness(2);
  object.addShape(std::move(head), sf::Vector2f(0, -38));

  object.setPosition(sf::Vector2f(400, 200));

  sf::CircleShape circle2(3);
  circle2.setOrigin(3, 3);
  circle2.setFillColor(sf::Color::Red);
  circle2.setPosition(400, 200);

  make_rectangles();

  sf::Clock frame_clock;
  // Main loop
  while (window.isOpen()) {
    // Event handling
    float dt = frame_clock.restart().asSeconds();
    float omega = 180;
    float d_theta = 0;
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
      }
    }
    if (window.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        d_theta = -omega * dt;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        d_theta = omega * dt;
      }
    }

    sf::Clock clock;

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    Vec2 mouse(mouse_pos.x, mouse_pos.y);
    float d = getDistance(p, mouse);
    float md = minimum_distance(a, b, mouse);
    object_rect.setPosition(mouse.x, mouse.y);
    object_rect.rotate(d_theta);
    int phase1 = clock.restart().asMicroseconds();

    object.setPosition(mouse.x, mouse.y);
    object.rotate(d_theta);
    circle2.setPosition(object.getPosition());

    if (object.collides_with(rect)) {
      rect.setFillColor(sf::Color::Red);
      object_rect.setFillColor(sf::Color::Red);
    } else {
      rect.setFillColor(sf::Color::Green);
      object_rect.setFillColor(sf::Color::Blue);
    }
    object_rect.setFillColor(sf::Color::Blue);
    for (const auto& rect : rectangles) {
      if (object.collides_with(rect)) {
        object_rect.setFillColor(sf::Color::Red);
      }
    }
    int phase2 = clock.restart().asMicroseconds();
    /////////////////////////////////////////////////////////
    // Clear the window
    window.clear(sf::Color::Black);
    for (const auto& rect : rectangles) {
      window.draw(rect);
    }

    draw_line(window, a, b);
    draw_point(window, p);
    draw_point(window, mouse);
    window.draw(rect);
    window.draw(object_rect);
    object.draw(window);
    window.draw(circle2);
    /////////////////////////////////////////////////////////

    int phase3 = clock.restart().asMicroseconds();

    std::string string = "";
    string = "times:\n";
    string += "p to mouse: " + std::to_string(md) + " us\n";
    string += "get mouse pos: " + std::to_string(phase2) + " us\n";
    string += "  draw shield: " + std::to_string(phase3) + " us\n";
    string += "    mouse pos: " + std::to_string((int)mouse_pos.x) + "," + std::to_string((int)mouse_pos.y) + " us\n";
    //    string += "   circle pos: " + std::to_string((int)object.circle_centre().x) + "," + std::to_string((int)object.circle_centre().y) + " us\n";
    //    string += "circle origin: " + std::to_string((int)object.circle_origin().x) + "," + std::to_string((int)object.circle_origin().y) + " us\n";
    // string += " circle angle: " + std::to_string((int)object..circle_origin().x) + "," + std::to_string((int)object.circle_origin().y) + " us\n";
    int total = phase1 + phase2 + phase3;
    text.setString(string);
    text.setPosition(600, 50);
    window.draw(text);
    // Display the window
    window.display();
  }

  return 0;
}
