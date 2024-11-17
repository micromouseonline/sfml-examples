#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include <memory>

#include "map.h"
#include "maze_constants.h"
#include "robot.h"
#include "robotview.h"
#include "util.h"
#include "walls.h"

/*********************************************************************************************************************/

float element_space = 10.0;
float map_view_size = 960.0f;
float sidebar_width = 300.0f;
float sidebar_height = 600.0f;
float mini_map_view_size = 300.0f;
float mini_map_view_port_size = sidebar_width;

float ui_title_font_height = 24.0f;
float ui_title_height = ui_title_font_height + 5.0f;
float window_width = map_view_size + sidebar_width + 3 * element_space;
float window_height = map_view_size + 2 * element_space;

int robot_width = 76;
int robot_height = 100;

sf::Color DustyRed(217, 87, 99);

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
  sf::RenderWindow window{sf::VideoMode(window_width, window_height), "808-wallmap", sf::Style::Titlebar + sf::Style::Close, settings};
  window.setFramerateLimit(60);

  sf::Clock up_time{};

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    std::cerr << "Unable to load font\n";
    exit(1);
  }

  Robot robot(robot_width, robot_height);
  robot.setPosition(270, 90 + 12 * 180);
  RobotView robot_view(robot);
  if (!robot_view.load_texture("./assets/images/mouse-76x100.png")) {
    std::cerr << "Unable to load texture\n";
    exit(1);
  };

  // create the tilemap from the level_map definition
  auto map = std::make_unique<TileMap>();
  map->set_font(font);
  if (!map->load("assets/images/maze-tiles-180x180.png", sf::Vector2u(180, 180), japan2007, 16, 16)) {
    std::cerr << "Unable to load maze tiles\n";
    exit(1);
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

  // Create a larger render texture
  // these are stored in the GPU and are fast
  // units are mm so 1 pixel/mm but it can be scaled at very little cost in the GPU
  auto renderTexture = std::make_unique<sf::RenderTexture>();
  if (!renderTexture->create(16 * 180, 16 * 180)) {
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

    robot.set_state(0);
    robot.setSpeed(0);
    if (window.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        robot.set_state(1);
        robot.rotate(3);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        robot.set_state(2);
        robot.rotate(-3);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        robot.set_state(3);
        robot.setSpeed(360);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        robot.set_state(3 + 4);  // because I can
        robot.setSpeed(-360);
      }
    }

    sf::Time time = deltaClock.restart();

    // update the objects on screen
    robot.update(time.asSeconds());
    robot_view.update();
    mini_map_view.setCenter(robot_view.getPosition().x, robot_view.getPosition().y);
    mini_map_view.setCenter(robot_view.getPosition().x, robot_view.getPosition().y);

    char buf[100];
    txt_robot_pose.setString(buf);

    /// and redraw the window
    window.clear();

    /// draw the frame around the maze
    window.setView(main_view);
    sf::RectangleShape frame(sf::Vector2f(map_view_size + 6, map_view_size + 6));
    frame.setFillColor(DustyRed);
    frame.setPosition(main_map_view_port_rect.left - 3.0f, main_map_view_port_rect.top - 3.0f);
    window.draw(frame);

    // render main map into a view
    window.setView(main_map_view);

    // Convert the mouse position to the map view
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    /***
     * the map view
     */
    float scale = map_view_size / renderTexture->getSize().x;
    int cellSize = scale * 180;
    int cellx = worldPos.x / cellSize;
    int celly = 16 - worldPos.y / cellSize;
    map->clear_colours();
    map->set_cell_colour(cellx, celly, sf::Color::Green);
    map->setScale(1, 1);
    robot_view.setScale(1, 1);
    renderTexture->clear();
    renderTexture->draw(*map);
    renderTexture->draw(robot_view);
    renderTexture->display();
    // now the render texture is drawn as a single sprite
    sf::Sprite map_sprite(renderTexture->getTexture());
    map_sprite.setScale(scale, scale);
    window.draw(map_sprite);

    // render minimap
    // mini_map_view.setRotation(robot.getRotation());
    /// Stuff must be drawn on both views. Still do not know why
    /// These may be two separate textures that both need filling
    window.setView(mini_map_view);
    map_sprite.setScale(1.0, 1.0);
    window.draw(map_sprite);

    mini_map_view.setCenter(robot_view.getPosition().x, robot_view.getPosition().y);
    // render UI stuff
    window.setView(main_view);
    time = deltaClock.restart();
    std::string txt = "Time: " + std::to_string(time.asMilliseconds()) + "\n";
    txt += "Mouse: " + std::to_string(mousePos.x) + "," + std::to_string(mousePos.y) + "\n";
    txt += "Map: " + std::to_string((int)worldPos.x) + "," + std::to_string((int)worldPos.y) + "\n";
    txt += "Cell: " + std::to_string((int)cellx) + "," + std::to_string((int)celly) + "\n";
    sprintf(buf, "Pose: %d,%d,%d", int(robot_view.getPosition().x), int(robot_view.getPosition().y), int(robot_view.getRotation()));
    txt_robot_pose.setString(buf);
    txt_robot_pose.setString(txt);
    txt_robot_pose.setPosition(mini_map_view_port_rect.left, mini_map_view_port_rect.top - txt_robot_pose.getLocalBounds().height);
    window.draw(txt_robot_pose);

    window.display();
  }

  return 0;
}
