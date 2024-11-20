#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include "maze.h"
#include "object.h"

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

// Function to calculate the intersection of a line with a rectangle edge
bool intersectLineWithEdge(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f q0, sf::Vector2f q1, sf::Vector2f& intersection) {
  sf::Vector2f r = p1 - p0;
  sf::Vector2f s = q1 - q0;
  float rxs = r.x * s.y - r.y * s.x;
  float qpxr = (q0.x - p0.x) * r.y - (q0.y - p0.y) * r.x;

  if (rxs == 0 && qpxr == 0) {
    return false;  // Lines are collinear
  }
  if (rxs == 0 && qpxr != 0) {
    return false;  // Lines are parallel
  }

  float t = ((q0.x - p0.x) * s.y - (q0.y - p0.y) * s.x) / rxs;
  float u = ((q0.x - p0.x) * r.y - (q0.y - p0.y) * r.x) / rxs;

  if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
    intersection = p0 + t * r;
    return true;
  }
  return false;
}

// Function to check for the first intersection
bool firstIntersection(sf::Vector2f lineStart, sf::Vector2f lineEnd, const sf::RectangleShape& rect, sf::Vector2f& intersectionPoint) {
  bool foundIntersection = false;
  float closestT = std::numeric_limits<float>::max();

  sf::FloatRect bounds = rect.getGlobalBounds();
  sf::Vector2f topLeft(bounds.left, bounds.top);
  sf::Vector2f topRight(bounds.left + bounds.width, bounds.top);
  sf::Vector2f bottomLeft(bounds.left, bounds.top + bounds.height);
  sf::Vector2f bottomRight(bounds.left + bounds.width, bounds.top + bounds.height);

  std::vector<sf::Vector2f> edgesStart = {topLeft, topRight, bottomRight, bottomLeft};
  std::vector<sf::Vector2f> edgesEnd = {topRight, bottomRight, bottomLeft, topLeft};

  for (size_t i = 0; i < edgesStart.size(); ++i) {
    sf::Vector2f currentIntersection;
    if (intersectLineWithEdge(lineStart, lineEnd, edgesStart[i], edgesEnd[i], currentIntersection)) {
      float t = (currentIntersection - lineStart).x / (lineEnd - lineStart).x;
      if (t < closestT) {
        closestT = t;
        intersectionPoint = currentIntersection;
        foundIntersection = true;
      }
    }
  }
  return foundIntersection;
}

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
    /// set the object colours to highlight collisions
    for (auto& wall : maze->walls) {
      for (int i = 0; i < 60; i++) {
        firstIntersection(sensor_start, sensor_end, wall, sensor_intersection);  // this is really slow
      }

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
    draw_line(window, sensor_start, sensor_intersection, sf::Color::Green);
    std::string string = "";
    string += " WASD keys move robot\n\n";
    string += "           FPS: " + std::to_string((int)(1.0f / dt)) + "\n";
    string += "     mouse pos: " + std::to_string((int)cg.x) + "," + std::to_string((int)cg.y) + "\n";
    string += "check and move: " + std::to_string(phase1) + " us\n";
    string += " sensor_length: " + std::to_string(sen_len) + " mm\n";
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
