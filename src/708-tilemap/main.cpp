#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include "map.h"

/*********************************************************************************************************************/

float element_space = 10.0;
float main_map_size = 960.0f;
float sidebar_width = 300.0f;
float sidebar_height = 600.0f;
float mini_map_view_size = 360.0f;
float mini_map_view_port_size = sidebar_width;

float ui_title_font_height = 24.0f;
float ui_title_height = ui_title_font_height + 5.0f;
float window_width = main_map_size + sidebar_width + 3 * element_space;
float window_height = main_map_size + 2 * element_space;

int robot_width = 77;
int robot_height = 100;
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
  settings.antialiasingLevel = 0;  // the number of multisamplings to use. 4 is probably fine
  /// do not let window resize
  sf::RenderWindow window{sf::VideoMode(window_width, window_height), "009 Views", sf::Style::Titlebar + sf::Style::Close, settings};
  window.setFramerateLimit(60);

  sf::Clock up_time{};

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    std::cerr << "Unable to load font\n";
    exit(1);
  }

  sf::Texture robot_sprite;
  if (!robot_sprite.loadFromFile("./assets/images/mouse-77x100.png")) {
    std::cerr << "Unable to load texture\n";
    exit(1);
  }

  sf::Sprite robot;
  robot.setTexture(robot_sprite);
  robot.setTextureRect(sf::IntRect(0, 0, 77, 100));
  robot.setOrigin(35, 60);
  robot.setPosition(90, 90);
  robot.setRotation(180.0);
  // robot.scale(0.5, 0.5);

  // create the tilemap from the level_map definition
  TileMap map;
  map.set_font(font);
  if (!map.load("assets/images/maze-tiles-180x180.png", sf::Vector2u(180, 180), japan2007, 16, 16)) {
    return -1;
  }

  /// Define the main regions for elements in the window
  /// This rectangle is the location of the main map in the window
  sf::FloatRect main_map_view_port_rect(element_space,                //
                                        element_space,                //
                                        main_map_size, main_map_size  //
  );

  /// This rectangle is the location of the mini map in the window
  sf::FloatRect mini_map_view_port_rect(main_map_size + 2 * element_space,                        //
                                        main_map_size + element_space - mini_map_view_port_size,  //
                                        mini_map_view_port_size, mini_map_view_port_size          //
  );

  sf::FloatRect visibleArea(0, 0, (float)window.getSize().x, (float)window.getSize().y);

  /// view the entire map
  sf::View main_map_view(sf::FloatRect(0, 0, main_map_size, main_map_size));
  main_map_view.setViewport(calculate_viewport(main_map_view_port_rect, visibleArea));

  /// view the mini map
  sf::View mini_map_view(sf::FloatRect(0, 0, mini_map_view_size, mini_map_view_size));
  mini_map_view.setViewport(calculate_viewport(mini_map_view_port_rect, visibleArea));

  sf::View main_view(visibleArea);  // the viewport is the whole window by default

  sf::Text txt_robot_pose("m;sjfs", font, 24);
  txt_robot_pose.setFillColor(sf::Color::White);
  txt_robot_pose.setPosition(mini_map_view_port_rect.left, mini_map_view_port_rect.top - 30);

  // Create a larger render texture
  sf::RenderTexture renderTexture;
  if (!renderTexture.create(3000, 3000)) {
    // Error handling
    return EXIT_FAILURE;
  }

  sf::Clock deltaClock;
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
    robot.setTextureRect(sf::IntRect(0 * robot_width, 0, robot_width, robot_height));
    if (window.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        robot.setTextureRect(sf::IntRect(1 * robot_width, 0, robot_width, robot_height));
        robot.rotate(3);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        robot.setTextureRect(sf::IntRect(2 * robot_width, 0, robot_width, robot_height));
        robot.rotate(-3);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        robot.setTextureRect(sf::IntRect(3 * robot_width, 0, robot_width, robot_height));
        v = 360;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        v = -360;
      }
    }
    char buf[100];

    sf::Time time = deltaClock.restart();
    float angle = robot.getRotation();
    float ds = v * time.asSeconds();
    float dx = std::cos((angle - 90) * 3.14 / 180) * v;
    float dy = std::sin((angle - 90) * 3.14 / 180) * v;
    if (robot.getPosition().x + dx < 1024 && robot.getPosition().x + dx > 0) {
      // robot.move(sf::Vector2f(dx, dy) * time.asSeconds());
    }
    robot.move(sf::Vector2f(dx, dy) * time.asSeconds());

    // float mx = sf::Mouse::getPosition(window).x;
    // float my = sf::Mouse::getPosition(window).y;
    // mx = std::max(mx, 32.0f);
    // my = std::max(my, 32.0f);

    // mx = std::min(mx, 31 * 32.0f);
    // my = std::min(my, 31 * 32.0f);

    txt_robot_pose.setString(buf);

    mini_map_view.setCenter(robot.getPosition().x, robot.getPosition().y);

    /// and redraw the window
    window.clear();

    // render main map
    // drawing is done into a view
    window.setView(main_map_view);
    map.setScale(1, 1);
    robot.setScale(1, 1);

    renderTexture.clear();
    renderTexture.draw(map);
    renderTexture.draw(robot);
    renderTexture.display();

    sf::Sprite map_sprite(renderTexture.getTexture());
    map_sprite.setScale(1.0 / 3.0, 1.0 / 3.0);
    window.draw(map_sprite);

    // render minimap
    // mini_map_view.setRotation(robot.getRotation());
    /// Stuff must be drawn on both views. Still do not know why
    window.setView(mini_map_view);
    map_sprite.setScale(1., 1.);
    mini_map_view.setCenter(robot.getPosition().x, robot.getPosition().y);
    // map.setScale(0.5, 0.5);
    // robot.setScale(1.0 / 3.0, 1.0 / 3.0);  // map.setScale(0.5, 0.5);
    window.draw(map_sprite);
    // window.draw(map);
    // window.draw(robot);

    // render UI stuff
    window.setView(main_view);
    time = deltaClock.restart();
    std::string txt = std::to_string(time.asMilliseconds());
    sprintf(buf, "Pose: %d,%d,%d", int(robot.getPosition().x), int(robot.getPosition().y), int(robot.getRotation()));
    txt_robot_pose.setString(buf);
    window.draw(txt_robot_pose);

    window.display();
  }

  return 0;
}
