#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include "maze.h"
#include "object.h"
#include "sensor.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
/***
 * This app tests the ability to do collision detection by geometry. The robot will have its
 * geometry defined by a number of shapes. In this case a single circle and a rectangle which
 * approximates the shape of Decimus 4 and Decimus 5. Polygons could also be used but are not
 * implemented here.
 *
 * The collision object has a nominal centre which corresponds to the robot centre of rotation.
 * The collision object is, then, defined in the robot coordinate frame. The shapes are added
 * with their position determined by an offset from that centre.
 *
 * In the utility file collisions.h, you will find various collision detection functions. For
 * a micromouse in the maze, we are concerned with collisions with axis aligned rectangles
 * representing the posts and walls.
 *
 * A half-size maze can have more than 2000 posts and walls. Even though a single collision
 * test might take only a couple of microseconds, attempting to test against all the posts and
 * walls will take several milliseconds.
 *
 * A better solution is to create a list of all the posts and walls in the current cel and its
 * eight neighbours and just test against them. That represents a maximum of about 30 objects.
 * that will bring the collision detect ion down to 150 microseconds making it feasible on
 * every millisecond tick of the robot control loop.
 *
 * Since a micromouse in the maze does not have any direct means to detect a collision, this
 * checking should be performed by the simulation code which might then send a message to the
 * robot or set a flag in shared state. Typical real collisions don't affect the robot in a
 * way that you might expect. Gyro controlled robots, for example, are able to slide along a
 * wall at an angle.
 *
 * The most appropriate response might be to halt the robot for log checking.
 *
 *
 */

//////////////////////////////////////////////////////////////////////////////////////////////////
///
///

//////////////////////////////////////////////////////////////////////////////////////////////////

void draw_line(sf::RenderTarget& target, sf::Vector2f pos, sf::Vector2f vec, sf::Color color = sf::Color::White) {
  sf::Vertex line[2];
  line[0].position = {pos.x, pos.y};
  line[0].color = color;
  line[1].position = {vec.x, vec.y};
  line[1].color = color;
  target.draw(line, 2, sf::Lines);
}

int main() {
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

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(20);                     // in pixels, not points!
  text.setFillColor(sf::Color(255, 0, 0, 255));  // it can be any colour//  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles

  /// Create a collision object representing the mouse geometry
  /// The components of the collision shape are added in order from bottom to top
  /// THe first one added will be the under he rest and is first checked
  CollisionGeometry collision_geometry(sf::Vector2f(0, 0));
  auto head = std::make_unique<sf::CircleShape>(38);
  head->setOrigin(38, 38);
  head->setFillColor(sf::Color(0, 66, 0, 255));
  collision_geometry.addShape(std::move(head), sf::Vector2f(0, -31));
  auto body = std::make_unique<sf::RectangleShape>(sf::Vector2f(76, 62));
  body->setFillColor(sf::Color(0, 76, 0, 255));
  body->setOrigin(38, 31);
  collision_geometry.addShape(std::move(body), sf::Vector2f(0, 0));

  collision_geometry.setPosition(90, 90);
  collision_geometry.setRotation(180);
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

  sf::Vector2f sensor_start(140, 40);
  sf::Vector2f sensor_end(255, 200);
  sf::Vector2f sensor_intersection(0, 0);

  sf::Vector2f lfs_offs = sf::Vector2f(-30, -40);
  sf::Vector2f lds_offs = sf::Vector2f(-10, -50);
  sf::Vector2f rds_offs = sf::Vector2f(+10, -50);
  sf::Vector2f rfs_offs = sf::Vector2f(+30, -40);

  float lfs_ang = -90 - 10;
  float lds_ang = -180 + 30;
  float rds_ang = 0 - 30;
  float rfs_ang = -90 + 10;

  Sensor sensor_lfs(collision_geometry.position() + sf::Vector2f(-30, -40), 0);
  Sensor sensor_lds(collision_geometry.position() + sf::Vector2f(-30, -40), 0);
  Sensor sensor_rds(collision_geometry.position() + sf::Vector2f(-30, -40), 0);
  Sensor sensor_rfs(collision_geometry.position() + sf::Vector2f(-30, -40), 0);

  sf::Clock frame_clock;
  // Main loop
  while (window.isOpen()) {
    // Event handling
    float dt = frame_clock.restart().asSeconds();
    float d_theta = 0;
    float d_s = 0;
    bool move = false;
    sf::Event event{};
    while (window.pollEvent(event)) {
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
    }

    sf::Clock clock;

    sf::RectangleShape block(sf::Vector2f(100, 100));
    block.setFillColor(sf::Color::Blue);
    block.setOrigin(50, 50);
    block.setPosition(200, 200);

    sf::Vector2f lfs_pos = rotatePoint(lfs_offs, {0, 0}, collision_geometry.angle());
    sf::Vector2f lds_pos = rotatePoint(lds_offs, {0, 0}, collision_geometry.angle());
    sf::Vector2f rds_pos = rotatePoint(rds_offs, {0, 0}, collision_geometry.angle());
    sf::Vector2f rfs_pos = rotatePoint(rfs_offs, {0, 0}, collision_geometry.angle());

    sensor_lfs.set_origin(collision_geometry.position() + lfs_pos);
    sensor_lds.set_origin(collision_geometry.position() + lds_pos);
    sensor_rds.set_origin(collision_geometry.position() + rds_pos);
    sensor_rfs.set_origin(collision_geometry.position() + rfs_pos);

    sensor_lfs.set_angle(collision_geometry.angle() + lfs_ang);
    sensor_lds.set_angle(collision_geometry.angle() + lds_ang);
    sensor_rds.set_angle(collision_geometry.angle() + rds_ang);
    sensor_rfs.set_angle(collision_geometry.angle() + rfs_ang);

    sf::Vector2f old_cg = collision_geometry.position();
    float old_angle = collision_geometry.angle();
    if (move) {
      float angle = collision_geometry.angle() + d_theta;
      float dx = std::cos((angle - 90.0f) * 3.14f / 180.0f) * d_s;
      float dy = std::sin((angle - 90.0f) * 3.14f / 180.0f) * d_s;
      sf::Vector2f movement(dx, dy);
      collision_geometry.rotate(d_theta);
      collision_geometry.setPosition(collision_geometry.position() + movement);
    }
    bool collided = false;
    float f;
    /// set the object colours to highlight collisions
    sf::Vector2f sensor_dir = sensor_end - sensor_start;
    for (auto& wall : maze->walls) {
      wall.setFillColor(sf::Color::Red);
      if (collision_geometry.collides_with(wall)) {
        collided = true;
        wall.setFillColor(sf::Color::Yellow);
        break;
      }
    }
    if (collided) {
      collision_geometry.setPosition(old_cg);
      collision_geometry.setRotation(old_angle);
      // d_s = 0;
      // d_theta = 0;
      // collided = false;
    }
    int phase1 = clock.restart().asMicroseconds();
    sf::Vector2f cg = collision_geometry.position();

    /////////////////////////////////////////////////////////
    sf::Vector2f v = sensor_intersection - sensor_start;
    int sen_len = int(std::sqrt(v.x * v.x + v.y * v.y));
    /////////////////////////////////////////////////////////
    window.clear(sf::Color::Black);
    maze->draw(window);
    collision_geometry.draw(window);

    window.draw(block);

    float lfs = sensor_lfs.draw(window, block);
    float lds = sensor_lds.draw(window, block);
    float rds = sensor_rds.draw(window, block);
    float rfs = sensor_rfs.draw(window, block);

    std::string string = "";
    string += " WASD keys move robot\n\n";
    string += "           FPS: " + std::to_string((int)(1.0f / dt)) + "\n";
    string += "     mouse pos: " + std::to_string((int)cg.x) + "," + std::to_string((int)cg.y) + "\n";
    string += "check and move: " + std::to_string(phase1) + " us\n";
    string += "    sensor_lfs: " + std::to_string(int(lfs)) + " mm\n";
    string += "    sensor_lds: " + std::to_string(int(lds)) + " mm\n";
    string += "    sensor_rds: " + std::to_string(int(rds)) + " mm\n";
    string += "    sensor_rfs: " + std::to_string(int(rfs)) + " mm\n";
    if (collided) {
      text.setFillColor(sf::Color::Yellow);
    } else {
      text.setFillColor(sf::Color::Red);
    }
    text.setString(string);
    text.setPosition(800, 10);
    window.draw(text);

    window.display();
  }

  return 0;
}
