#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <thread>
#include "application.h"
#include "maze.h"
#include "object.h"
#include "robot-control.h"
#include "robot-display.h"
#include "sensor.h"
#include "thread-safe-queue.h"
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
/***
 * This ap starts to take the 015 example and convert it to a more generic application structure.
 *
 * There are more generic classes used to assemble the application. In particular, main()
 * has little to do besides initialise the application and then run it. All the actual work
 * is done in the application class which manages its own window(s) and resources.
 *
 * The structure is derived from code associated with the book
 * "SFML Game Development by Example" by Maxime Lévesque
 *
 * The repository for the book code is at:
 * https://github.com/SFML/SFML-Game-Development-By-Example
 *
 */

//////////////////////////////////////////////////////////////////////////////////////////////////
///
///

//////////////////////////////////////////////////////////////////////////////////////////////////

struct RobotState {
  sf::Vector2f pos{96.0f, 96.0f};
  int angle = 0.0f;
  int sensor_half_angle = 5;
  int front_sensor_angle = 10;
  int side_sensor_angle = 30;
  sf::Vector2f lfs_offs = sf::Vector2f(-30, -40);
  sf::Vector2f lds_offs = sf::Vector2f(-10, -50);
  sf::Vector2f rds_offs = sf::Vector2f(+10, -50);
  sf::Vector2f rfs_offs = sf::Vector2f(+30, -40);
};

RobotState g_robot_state;

CollisionGeometry g_robot(sf::Vector2f(0, 0));
Sensor sensor_lfs(g_robot.position() + g_robot_state.lfs_offs, g_robot.angle(), (float)g_robot_state.sensor_half_angle, 64);
Sensor sensor_lds(g_robot.position() + g_robot_state.lds_offs, g_robot.angle(), (float)g_robot_state.sensor_half_angle, 64);
Sensor sensor_rds(g_robot.position() + g_robot_state.rds_offs, g_robot.angle(), (float)g_robot_state.sensor_half_angle, 64);
Sensor sensor_rfs(g_robot.position() + g_robot_state.rfs_offs, g_robot.angle(), (float)g_robot_state.sensor_half_angle, 64);

void configure_sensor_geometry(CollisionGeometry& robot) {
  sensor_lfs.set_angle(robot.angle());
  sensor_lfs.set_half_angle((float)g_robot_state.sensor_half_angle);
  sensor_lds.set_angle(robot.angle());
  sensor_lds.set_half_angle((float)g_robot_state.sensor_half_angle);
  sensor_rds.set_angle(robot.angle());
  sensor_rds.set_half_angle((float)g_robot_state.sensor_half_angle);
  sensor_rfs.set_angle(robot.angle());
  sensor_rfs.set_half_angle((float)g_robot_state.sensor_half_angle);

  /// Update the sensor geometry. This is done after we have
  /// decided if we have collided or not so the angles and positions are correct
  /// It is all pretty cumbersome for now
  sf::Vector2f lfs_pos = rotatePoint(g_robot_state.lfs_offs, {0, 0}, robot.angle());
  sf::Vector2f lds_pos = rotatePoint(g_robot_state.lds_offs, {0, 0}, robot.angle());
  sf::Vector2f rds_pos = rotatePoint(g_robot_state.rds_offs, {0, 0}, robot.angle());
  sf::Vector2f rfs_pos = rotatePoint(g_robot_state.rfs_offs, {0, 0}, robot.angle());

  sensor_lfs.set_origin(robot.position() + lfs_pos);
  sensor_lds.set_origin(robot.position() + lds_pos);
  sensor_rds.set_origin(robot.position() + rds_pos);
  sensor_rfs.set_origin(robot.position() + rfs_pos);
  float lfs_ang = -90 - g_robot_state.front_sensor_angle;
  float lds_ang = -180 + g_robot_state.side_sensor_angle;
  float rds_ang = 0 - g_robot_state.side_sensor_angle;
  float rfs_ang = -90 + g_robot_state.front_sensor_angle;
  sensor_lfs.set_angle(robot.angle() + lfs_ang);
  sensor_lds.set_angle(robot.angle() + lds_ang);
  sensor_rds.set_angle(robot.angle() + rds_ang);
  sensor_rfs.set_angle(robot.angle() + rfs_ang);
}

/// ChatGPT main suggestion:
int mainx() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "Robot Simulation");
  ThreadSafeQueue logQueue;

  Robot robot{0, 0, 0};
  Maze maze;
  SensorData sensorData;
  RobotControl robotControl(robot, maze, sensorData, logQueue);
  RobotDisplay robotDisplay(robotControl, window, logQueue);

  // Run the robot control logic in a separate thread
  std::thread controlThread([&robotControl]() { robotControl.run(); });

  // Run the display logic in the main thread
  robotDisplay.run();

  // Join threads before exiting
  controlThread.join();
  return 0;
}
int main() {
  // Program entry point.
  Application app;
  app.GetWindow()->SetTitle("HARRY THE ROBOT");
  while (!app.GetWindow()->IsDone()) {
    // Application loop.
    app.HandleInput();
    app.Update();
    app.Render();
    app.RestartClock();
  }
  return 0;
  // Create the window
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multi-samplings to use. 4 is probably fine
  sf::RenderWindow window(sf::VideoMode(1200, 800), WINDOW_TITLE, sf::Style::Default, settings);

  window.setFramerateLimit(60);
  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }
  if (!ImGui::SFML::Init(window)) {
    std::cerr << "failed to initialose ImGui\n";
    exit(1);
  };
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(20);                     // in pixels, not points!
  text.setFillColor(sf::Color(255, 0, 0, 255));  // it can be any colour//  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles

  /// Create a collision object representing the mouse geometry
  /// The components of the collision shape are added in order from bottom to top
  /// THe first one added will be the under he rest and is first checked
  auto head = std::make_unique<sf::CircleShape>(38);
  head->setOrigin(38, 38);
  head->setFillColor(sf::Color(0, 66, 0, 255));
  g_robot.addShape(std::move(head), sf::Vector2f(0, -31));
  auto body = std::make_unique<sf::RectangleShape>(sf::Vector2f(76, 62));
  body->setFillColor(sf::Color(0, 76, 0, 255));
  body->setOrigin(38, 31);
  g_robot.addShape(std::move(body), sf::Vector2f(0, 0));

  g_robot.setPosition(96, 96);
  g_robot.setRotation(180);
  std::unique_ptr<Maze> maze = std::make_unique<Maze>();
  maze->add_posts(5, 5);
  /// note that  this is a simple demo, nothing stops duplicate walls
  for (int i = 0; i < 4; i++) {
    maze->add_wall(i, 0, NORTH);
    maze->add_wall(i, 3, SOUTH);
    maze->add_wall(0, i, WEST);
    maze->add_wall(3, i, EAST);
  }
  maze->add_wall(0, 0, EAST);
  maze->add_wall(2, 2, EAST);
  maze->add_wall(2, 3, WEST);
  maze->add_wall(1, 1, SOUTH);
  maze->add_wall(2, 0, SOUTH);
  maze->add_wall(1, 1, EAST);
  maze->add_wall(0, 2, EAST);

  float v = 180;
  float omega = 180;

  sf::Clock frame_clock;
  // Main loop
  while (window.isOpen()) {
    // Event handling
    sf::Time frame_time = frame_clock.restart();
    float dt = frame_time.asSeconds();
    float d_theta = 0;
    float d_s = 0;
    bool move = false;
    sf::Event event{};
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(window, event);
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
      }
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }
    ImGui::SFML::Update(window, frame_time);
    ImGui::Begin("Sensors");
    /// TODO: figure out how to use ImGui rotate/move the robot, overriding the current
    ///       motion
    /// ImGui::SliderInt("Robot Angle", &g_robot_state.angle, 0, 360);
    ImGui::SliderInt("Sensor Half Angle", &g_robot_state.sensor_half_angle, 1.0f, 30.0f);
    ImGui::SliderInt("Side Sensor Angle", &g_robot_state.side_sensor_angle, 1.0f, 60.0f);
    ImGui::SliderInt("Front Sensor Angle", &g_robot_state.front_sensor_angle, 1.0f, 30.0f);
    ImGui::End();

    if (window.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        d_theta = -omega * dt;
        // rotate = true;
        move = true;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        d_theta = omega * dt;
        move = true;
        // rotate = true;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        d_s = v * dt;
        move = true;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        d_s = -v * dt;
        move = true;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
        d_s /= 10;
        d_theta /= 10;
      }
    }

    sf::Clock clock;

    sf::Vector2f old_cg = g_robot.position();
    float old_angle = g_robot.angle();
    if (move) {
      float angle = g_robot.angle() + d_theta;
      float dx = std::cos((angle - 90.0f) * 3.14f / 180.0f) * d_s;
      float dy = std::sin((angle - 90.0f) * 3.14f / 180.0f) * d_s;
      sf::Vector2f movement(dx, dy);
      g_robot.rotate(d_theta);
      g_robot.setPosition(g_robot.position() + movement);
    }
    bool collided = false;
    /// set the object colours to highlight collisions
    for (auto& wall : maze->walls) {
      wall.setFillColor(sf::Color::Red);
      if (g_robot.collides_with(wall)) {
        collided = true;
        wall.setFillColor(sf::Color::Yellow);
        break;
      }
    }
    g_robot.set_colour(sf::Color::White);
    if (collided) {
      g_robot.set_colour(sf::Color::Red);
      g_robot.setPosition(old_cg);
      g_robot.setRotation(old_angle);
    }
    /////
    /// Robot is now in place
    /// so we update the sensor geometry

    g_robot_state.angle = (int)g_robot.angle();

    configure_sensor_geometry(g_robot);

    sf::Int64 phase1 = clock.restart().asMicroseconds();
    sensor_lfs.update(maze->walls);
    sensor_lds.update(maze->walls);
    sensor_rds.update(maze->walls);
    sensor_rfs.update(maze->walls);

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    window.clear(sf::Color::Black);
    maze->draw(window);
    g_robot.draw(window);
    sensor_lfs.draw(window);
    sensor_lds.draw(window);
    sensor_rds.draw(window);
    sensor_rfs.draw(window);

    std::string string;
    string += " WASD keys move robot\n\n";
    string += "           FPS: " + std::to_string((int)(1.0f / dt)) + "\n";
    string += "     mouse pos: " + std::to_string((int)g_robot.position().x) + "," + std::to_string((int)g_robot.position().y) + "\n";
    string += "     mouse ang: " + std::to_string(g_robot.angle()) + "\n";
    string += "check and move: " + std::to_string(phase1) + " us\n";
    string += "    sensor_lfs: " + std::to_string(int(sensor_lfs.power())) + " -> " + std::to_string(int(sensor_lfs.distance())) + " mm\n";
    string += "    sensor_lds: " + std::to_string(int(sensor_lds.power())) + " -> " + std::to_string(int(sensor_lds.distance())) + " mm\n";
    string += "    sensor_rds: " + std::to_string(int(sensor_rds.power())) + " -> " + std::to_string(int(sensor_rds.distance())) + " mm\n";
    string += "    sensor_rfs: " + std::to_string(int(sensor_rfs.power())) + " -> " + std::to_string(int(sensor_rfs.distance())) + " mm\n";
    if (collided) {
      text.setFillColor(sf::Color::Yellow);
    } else {
      text.setFillColor(sf::Color::Red);
    }
    text.setString(string);
    text.setPosition(800, 10);
    window.draw(text);
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();

  return 0;
}
