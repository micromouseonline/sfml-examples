#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>

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

  // Bresenham's line algorithm for efficient traversal
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
      return {(float)x1, (float)y1};
    }

    color = getColorAtPixel(image, x0, y0);
    if (color == wall_colour) {
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

/***
 * simulate a single sensor by casting out a fan of rays.
 * @param renderTarget  - the place we will draw the rays
 * @param image - holds the map
 * @param pos  - the position of the emitter
 * @param angle - the angle of the centre of the fan
 * @param width - the angle subtended by the fan
 * @param steps - how many rays to use in the fan
 * @return - the average distance of all the rays
 */
float sensor(sf::RenderTarget& renderTarget, sf::Image& image, sf::Vector2f pos, float angle, float width, int steps) {
  float power = 0;
  float start = angle - width / 2;
  float inc = width / steps;
  sf::Vertex line[] = {sf::Vertex(pos), sf::Vertex(pos)};
  line[0].color = sf::Color(255, 128, 0, 196);
  for (int i = 0; i < steps; i++) {
    sf::Vector2f hitPosition = castRay(image, sf::Color::Red, pos, start);
    float d = distance(pos, hitPosition);
    float p = (800.0 / d);
    p = p * p;
    power += p;  // distance(pos, hitPosition);
    line[1].position = hitPosition;
    line[1].color = sf::Color(255, 128, 0, 255 - d);
    renderTarget.draw(line, 2, sf::Lines);
    start += inc;
  }
  float average = power / (float)steps;
  return std::min(average, 1024.0f);
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

  sf::Sprite mouse;
  mouse.setTexture(mouse_texture);
  mouse.setTextureRect(sf::IntRect{0, 0, 77, 100});
  mouse.setOrigin(38, 60);
  mouse.setPosition(286, 466);

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

  // Main loop
  sf::Clock clock;
  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();
    // Event handling
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    std::string string = "";
    sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

    // Clear the window
    window.clear(sf::Color::Black);
    window.draw(maze_map);
    mouse.setPosition(mousePosition);
    window.draw(mouse);

    sf::Image image = map_texture.getTexture().copyToImage();
    sf::Clock clock;
    float power = 0;
    /// Todo: this all needs automating in a class
    float lfs = sensor(window, image, mouse.getPosition() + sf::Vector2f(-30, -40), -90 - 10, 5, 5 * 10);
    float lds = sensor(window, image, mouse.getPosition() + sf::Vector2f(-10, -55), -180 + 30, 5, 5 * 10);
    float rds = sensor(window, image, mouse.getPosition() + sf::Vector2f(+10, -55), 0 - 30, 5, 5 * 10);
    float rfs = sensor(window, image, mouse.getPosition() + sf::Vector2f(+30, -40), -90 + 10, 5, 5 * 10);
    string = std::to_string(clock.restart().asMicroseconds()) + " us\n";
    sf::Vector2f hitPosition = castRay(map_texture.getTexture().copyToImage(), sf::Color(), mousePosition, 270);
    string += "Mouse: " + std::to_string((int)mousePosition.x) + ", " + std::to_string((int)mousePosition.y) + "\n";
    string += "Hit: " + std::to_string((int)hitPosition.x) + ", " + std::to_string((int)hitPosition.y) + "\n";
    string += "LFS: " + std::to_string((int)(lfs)) + "\n";
    string += "LDS: " + std::to_string((int)(lds)) + "\n";
    string += "RDS: " + std::to_string((int)(rds)) + "\n";
    string += "RFS: " + std::to_string((int)(rfs)) + "\n";
    text.setString(string);
    text.setPosition(1000, 50);
    window.draw(text);

    // Display the window
    window.display();
  }

  return 0;
}
