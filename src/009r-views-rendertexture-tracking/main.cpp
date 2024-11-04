#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include "map.h"

/*********************************************************************************************************************/

float element_space = 10.0;
float map_view_size = 960.0f;
float sidebar_width = 300.0f;
float sidebar_height = 600.0f;
float mini_map_view_size = 150.0f;
float mini_map_view_port_size = sidebar_width;

float ui_title_font_height = 24.0f;
float ui_title_height = ui_title_font_height + 5.0f;
float window_width = map_view_size + sidebar_width + 3 * element_space;
float window_height = map_view_size + 2 * element_space;

/***
 * Calculate the viewport coordinates from the window coordinates
 * viewport is a rectanglular region in pixel coordinates
 * window is the window in which we want to see the viewport (in pixel coordinates)
 *
 * returns a normalised viewport using fractions of the window
 *
 */
sf::FloatRect calculate_viewport(const sf::FloatRect& viewport_rect, const sf::FloatRect& window_rect) {
  const float x = viewport_rect.left / window_rect.width;
  const float y = viewport_rect.top / window_rect.height;
  const float w = viewport_rect.width / window_rect.width;
  const float h = viewport_rect.height / window_rect.height;
  return {x, y, w, h};
}

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  /// do not let window resize
  sf::RenderWindow window{sf::VideoMode(window_width, window_height), WINDOW_TITLE, sf::Style::Titlebar + sf::Style::Close, settings};
  window.setFramerateLimit(60);
  std::vector<sf::RectangleShape> walls;
  walls.resize(10000);
  for (int i = 0; i < walls.size(); i++) {
    walls[i].setSize(sf::Vector2f(32, 32));
    walls[i].setPosition(rand() % 1000, rand() % 1000);
    walls[i].setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
    walls[i].setOrigin(16, 16);
    walls[i].rotate(rand() % 360);
  }
  sf::Clock clock{};

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    std::cerr << "Unable to load font\n";
    exit(1);
  }

  sf::Texture robot_sprite;
  if (!robot_sprite.loadFromFile("./assets/images/mouse-8x1.png")) {
    std::cerr << "Unable to load texture\n";
    exit(1);
  }

  sf::RenderTexture maze_render_texture;
  if (!maze_render_texture.create(3000, 1500)) {
    std::cerr << "Unable to create maze render texture\n";
    exit(1);
  }

  sf::Sprite robot;
  robot.setTexture(robot_sprite);
  robot.setTextureRect(sf::IntRect(32, 0, 28, 32));
  robot.setOrigin(13, 20);
  robot.setPosition(15 + 30 * 5, (32 * 30) - (15 + 8 * 30));
  robot.scale(0.5, 0.5);

  // create the tilemap from the level_map definition
  TileMap map;
  if (!map.load("assets/images/maze-hf.png", sf::Vector2u(30, 30), level_map, 32, 32)) {
    return -1;
  }

  /// Define the main regions for elements in the window
  /// This rectangle is the location of the main map in the window
  sf::FloatRect main_map_view_port_rect(element_space,                //
                                        element_space,                //
                                        map_view_size, map_view_size  //
  );

  /// This rectangle is the location of the mini map in the window
  sf::FloatRect mini_map_view_port_rect(map_view_size + 2 * element_space,                        //
                                        map_view_size + element_space - mini_map_view_port_size,  //
                                        mini_map_view_port_size, mini_map_view_port_size          //
  );

  sf::FloatRect visibleArea(0, 0, (float)window.getSize().x, (float)window.getSize().y);

  /// view the entire map
  sf::View main_map_view(sf::FloatRect(0, 0, map_view_size, map_view_size));
  main_map_view.setViewport(calculate_viewport(main_map_view_port_rect, visibleArea));

  /// view the mini map
  sf::View mini_map_view(sf::FloatRect(0, 0, mini_map_view_size, mini_map_view_size));
  mini_map_view.setViewport(calculate_viewport(mini_map_view_port_rect, visibleArea));

  sf::View main_view(visibleArea);  // the viewport is the whole window by default

  sf::Text txt_robot_pose("m;sjfs", font, 24);
  txt_robot_pose.setFillColor(sf::Color::White);
  txt_robot_pose.setPosition(mini_map_view_port_rect.left, mini_map_view_port_rect.top - 30);
  sf::Clock deltaClock;
  int frame_count = 0;
  while (window.isOpen()) {
    /// process all the inputs
    sf::Event event{};
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed: {
          window.close();
          break;
        }
        case sf::Event::Resized: {  // seems to be generated when window is created as well
          /// get the new size of the window
          visibleArea = sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height);
          /// ensure the viewports scale correctly - comment these to scale everything with the window
          /// TODO: work out how to scale with same aspect ratio.
          main_map_view.setViewport(calculate_viewport(main_map_view_port_rect, visibleArea));
          mini_map_view.setViewport(calculate_viewport(mini_map_view_port_rect, visibleArea));
          main_view = sf::View(visibleArea);  // the viewport is the whole window by default
          break;
        }
        default:
          break;
      }
    }

    float v = 0;
    if (frame_count > 0) {
      frame_count--;
    }
    robot.setTextureRect(sf::IntRect(0, 0, 28, 32));
    if (window.hasFocus()) {
      if (frame_count == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        robot.setTextureRect(sf::IntRect(32, 0, 28, 32));
        robot.rotate(-45);
        frame_count = 60;
      }
      if (frame_count == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        robot.setTextureRect(sf::IntRect(64, 0, 28, 32));
        robot.rotate(45);
        frame_count = 60;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        robot.setTextureRect(sf::IntRect(96, 0, 28, 32));
        v = 100;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        v = -100;
      }
    }
    char buf[100];

    sf::Time time = deltaClock.restart();
    float angle = robot.getRotation();
    float ds = v * time.asSeconds();
    float dx = std::cos((angle - 90) * 3.14 / 180) * v;
    float dy = std::sin((angle - 90) * 3.14 / 180) * v;
    if (robot.getPosition().x + dx < 1024 && robot.getPosition().x + dx > 0) {
      robot.move(sf::Vector2f(dx, dy) * time.asSeconds());
    }

    // float mx = sf::Mouse::getPosition(window).x;
    // float my = sf::Mouse::getPosition(window).y;
    // mx = std::max(mx, 32.0f);
    // my = std::max(my, 32.0f);

    // mx = std::min(mx, 31 * 32.0f);
    // my = std::min(my, 31 * 32.0f);

    mini_map_view.setCenter(robot.getPosition().x, robot.getPosition().y);

    /// and redraw the window
    window.clear();
    clock.restart();
    window.setView(main_view);
    maze_render_texture.clear(sf::Color::Blue);
    maze_render_texture.draw(map);
    maze_render_texture.draw(robot);
    maze_render_texture.display();
    for (auto wall : walls) {
      // wall.rotate(robot.getRotation());
      // window.draw(wall);
    }
    const sf::Texture& texture = maze_render_texture.getTexture();
    // texture

    // draw it to the window
    sf::Sprite maze_map(texture);

    // drawing is done into a view
    window.setView(main_map_view);
    window.draw(map);
    window.draw(robot);
    window.draw(maze_map);

    // mini_map_view.setRotation(robot.getRotation());
    /// Stuff must be drawn on both views. Still do not know why
    /// TODO: is it because a vew is a copy of something?
    window.setView(mini_map_view);
    window.draw(map);
    window.draw(robot);
    window.setView(main_view);
    time = clock.restart();
    sprintf(buf, "Pose: %d,%d,%03d (%02ld ms)", int(robot.getPosition().x), int(robot.getPosition().y), int(robot.getRotation()), time.asMilliseconds());
    txt_robot_pose.setString(buf);
    window.draw(txt_robot_pose);

    window.display();
  }

  return 0;
}
