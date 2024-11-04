#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include "raycaster.h"
#include "sensor.h"
#include "utils.h"

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
  sf::RenderWindow window(sf::VideoMode(1200, 900), WINDOW_TITLE, sf::Style::Titlebar + sf::Style::Close, settings);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }
  // Create a RenderTexture
  sf::RenderTexture map_texture;
  if (!map_texture.create(1200, 900)) {
    // Error handling
    return -1;
  }

  sf::Cursor cursor;
  cursor.loadFromSystem(sf::Cursor::Cross);
  window.setMouseCursor(cursor);

  /// and make a mouse sprite for more pretty
  sf::Texture mouse_texture;
  mouse_texture.loadFromFile("./assets/images/mouse-77x100.png");

  sf::Sprite robot;
  robot.setTexture(mouse_texture);
  robot.setTextureRect(sf::IntRect{0, 0, 77, 100});
  robot.setOrigin(39, 60);
  robot.setPosition(106 + 2 * 180, 105 + 2 * 180);
  robot.setRotation(270);

  /// Text uses the sf::Text class
  sf::Text text;
  text.setString("VVVVVVV - - - - - ________ / / / / / /");
  text.setFont(font);
  text.setCharacterSize(24);                     // in pixels, not points!
  text.setFillColor(sf::Color(255, 0, 0, 255));  // it can be any colour//  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles
  text.setPosition(80, 10);

  map_texture.clear(sf::Color(32, 32, 32));
  create_obstacles(map_texture);
  map_texture.draw(text);
  map_texture.display();
  // give the RenderTexture to a sprite
  sf::Sprite maze_map(map_texture.getTexture());

  sf::Vector2f lfs_offs = sf::Vector2f(-30, -40);
  sf::Vector2f lds_offs = sf::Vector2f(-10, -55);
  sf::Vector2f rds_offs = sf::Vector2f(+10, -55);
  sf::Vector2f rfs_offs = sf::Vector2f(+30, -40);

  float lfs_ang = -90 - 10;
  float lds_ang = -180 + 30;
  float rds_ang = 0 - 30;
  float rfs_ang = -90 + 10;

  sf::Image image = map_texture.getTexture().copyToImage();
  Sensor sensor_lfs(robot.getPosition() + sf::Vector2f(-30, -40), 0);
  Sensor sensor_lds(robot.getPosition() + sf::Vector2f(-30, -40), 0);
  Sensor sensor_rds(robot.getPosition() + sf::Vector2f(-30, -40), 0);
  Sensor sensor_rfs(robot.getPosition() + sf::Vector2f(-30, -40), 0);

  // Main loop
  sf::Clock deltaClock;
  while (window.isOpen()) {
    // Event handling
    sf::Event event;
    while (window.pollEvent(event)) {
      bool should_close = false;
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        should_close = true;
      }
      if (event.type == sf::Event::Closed) {
        should_close = true;
      };
      if (should_close) {
        window.close();
      }
    }
    float v = 0;
    float w = 0;
    if (window.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        w = -90.0f;
        //        robot.rotate(-1);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        w = 90.0f;
        //        robot.rotate(1);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        v = 100;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        v = -100;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        v = v / 10;
        w = w / 50;
      }
    }

    std::string string = "";
    sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

    sf::Time time = deltaClock.restart();
    float theta = w * time.asSeconds();
    robot.rotate(theta);
    float angle = robot.getRotation();
    float ds = v * time.asSeconds();
    float dx = std::cos((angle - 90) * 3.14 / 180) * ds;
    float dy = std::sin((angle - 90) * 3.14 / 180) * ds;
    if (robot.getPosition().x + dx < 1024 && robot.getPosition().x + dx > 0) {
      robot.move(sf::Vector2f(dx, dy));
    }
    // Clear the window
    window.clear(sf::Color::Black);
    window.draw(maze_map);
    //    robot.setPosition(mousePosition);
    window.draw(robot);

    sf::Clock clock;

    sf::Vector2f lfs_pos = rotatePoint(lfs_offs, {0, 0}, robot.getRotation());
    sf::Vector2f lds_pos = rotatePoint(lds_offs, {0, 0}, robot.getRotation());
    sf::Vector2f rds_pos = rotatePoint(rds_offs, {0, 0}, robot.getRotation());
    sf::Vector2f rfs_pos = rotatePoint(rfs_offs, {0, 0}, robot.getRotation());
    sensor_lfs.set_origin(robot.getPosition() + lfs_pos);
    sensor_lds.set_origin(robot.getPosition() + lds_pos);
    sensor_rds.set_origin(robot.getPosition() + rds_pos);
    sensor_rfs.set_origin(robot.getPosition() + rfs_pos);

    sensor_lfs.set_angle(robot.getRotation() + lfs_ang);
    sensor_lds.set_angle(robot.getRotation() + lds_ang);
    sensor_rds.set_angle(robot.getRotation() + rds_ang);
    sensor_rfs.set_angle(robot.getRotation() + rfs_ang);

    float lfs = sensor_lfs.draw(window, image);
    float lds = sensor_lds.draw(window, image);
    float rds = sensor_rds.draw(window, image);
    float rfs = sensor_rfs.draw(window, image);

    string = std::to_string(clock.restart().asMicroseconds()) + " us\n";
    string += "Robot posn: " + std::to_string((int)robot.getPosition().x) + ", " + std::to_string((int)robot.getPosition().y) + "\n";
    char buffer[20];
    std::snprintf(buffer, sizeof(buffer), "%.1f", robot.getRotation());
    string += "Robot angle: " + std::string(buffer) + "\n";
    string += "Mouse: " + std::to_string((int)sf::Mouse::getPosition().x) + ", " + std::to_string((int)sf::Mouse::getPosition().y) + "\n";
    string += "LFS: " + std::to_string((int)(lfs)) + "\n";
    string += "LDS: " + std::to_string((int)(lds)) + "\n";
    string += "RDS: " + std::to_string((int)(rds)) + "\n";
    string += "RFS: " + std::to_string((int)(rfs)) + "\n";
    text.setString(string);
    text.setPosition(900, 50);
    window.draw(text);

    // Display the window
    window.display();
  }

  return 0;
}
