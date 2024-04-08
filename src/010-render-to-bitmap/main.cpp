#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>

// calculates dot product of 2 vectors
inline float dot(const sf::Vector2f& lv, const sf::Vector2f& rv) {
  return lv.x * rv.x + lv.y * rv.y;
}

// Returns length of a given vector
inline float length(const sf::Vector2f& source) {
  return std::sqrt(dot(source, source));
}

// Returns distance between points
inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

// Returns a given vector with its length normalized to 1
inline sf::Vector2f normalise(sf::Vector2f source) {
  float length = std::sqrt(dot(source, source));
  if (length != 0)
    source /= length;
  return source;
}

bool isBackground(const sf::Color& color) {
  // Define your background color here
  return color == sf::Color::Blue;
}

bool isWithinBounds(const sf::Vector2u& size, int x, int y) {
  return x >= 0 && x < static_cast<int>(size.x) && y >= 0 && y < static_cast<int>(size.y);
}

sf::Color getColorAtPixel(const sf::Image& image, int x, int y) {
  return isWithinBounds(image.getSize(), x, y) ? image.getPixel(x, y) : sf::Color::Transparent;
}

sf::Vector2f castRay(const sf::Image& image, const sf::Vector2f& mousePosition, float angle) {
  // Convert angle to radians
  angle = angle * (3.14159 / 180);
  float range = 254;
  sf::Vector2f rayDirection(range * cos(angle), range * sin(angle));

  sf::Vector2f pixelPosition = mousePosition + rayDirection;
  sf::Color color = getColorAtPixel(image, static_cast<int>(pixelPosition.x), static_cast<int>(pixelPosition.y));

  // Bresenham's line algorithm for efficient traversal
  int x0 = static_cast<int>(mousePosition.x);
  int y0 = static_cast<int>(mousePosition.y);
  int x1 = static_cast<int>(pixelPosition.x);
  int y1 = static_cast<int>(pixelPosition.y);

  int dx = std::abs(x1 - x0);
  int dy = std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;

  while (x0 != x1 || y0 != y1) {
    if (!isWithinBounds(image.getSize(), x0, y0)) {
      break;
    }

    color = getColorAtPixel(image, x0, y0);
    //    if (!isBackground(color)) {
    if (color == sf::Color(255, 0, 0)) {
      return sf::Vector2f(x0, y0);
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

  return sf::Vector2f{(float)x0, (float)y0};
}

float sensors(sf::RenderTarget& renderTarget, sf::Image& image, sf::Vector2f pos, float angle, float width, int steps) {
  float power = 0;
  float start = angle - width / 2;
  float inc = width / steps;
  for (int i = 0; i < steps; i++) {
    sf::Vector2f hitPosition = castRay(image, pos, start);
    int d = distance(pos, hitPosition);
    power += distance(pos, hitPosition);

    // Draw a line representing the ray
    sf::Vertex line[] = {sf::Vertex(pos), sf::Vertex(hitPosition)};
    line[0].color = sf::Color(255, 128, 0, 196);
    line[1].color = sf::Color(255, 128, 0, 255 - d);
    renderTarget.draw(line, 2, sf::Lines);
    start += inc;
  }
  return power / steps;
}

int main() {
  // Create the window
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML RenderTexture Example", sf::Style::Default, settings);

  // Create a RenderTexture
  sf::RenderTexture renderTexture;
  if (!renderTexture.create(400, 300)) {
    // Error handling
    return -1;
  }
  sf::Cursor cursor;
  cursor.loadFromSystem(sf::Cursor::Cross);
  window.setMouseCursor(cursor);

  // Clear the RenderTexture
  renderTexture.clear(sf::Color::Black);
  // Draw some objects to the RenderTexture
  sf::RectangleShape post(sf::Vector2f(40, 40));
  post.setFillColor(sf::Color::Red);
  //    circle.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
  post.setOrigin(20, 20);
  post.setPosition(150, 150);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }

  /// add a rectangle to illustrate positioning
  sf::RectangleShape rect(sf::Vector2f(100, 100));
  rect.setFillColor(sf::Color(32, 32, 32));

  /// Text uses the sf::Text class
  sf::Text text;
  text.setString("Here is some text");
  text.setFont(font);
  text.setCharacterSize(24);                       // in pixels, not points!
  text.setFillColor(sf::Color(255, 255, 0, 255));  // it can be any colour
                                                   //  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles
  text.setPosition(80, 10);

  // Main loop
  while (window.isOpen()) {
    // Event handling
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    std::string s = "";
    sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
    s += "Mouse: " + std::to_string((int)mousePosition.x) + ", " + std::to_string((int)mousePosition.y) + "\n";
    //    s += "Hit: " + std::to_string((int)hitPosition.x) + ", " + std::to_string((int)hitPosition.y) + "\n";
    text.setPosition(50, 0);
    text.setString(s);
    // Clear the window

    renderTexture.clear(sf::Color::Blue);
    renderTexture.draw(post);
    renderTexture.draw(text);

    // Display the RenderTexture
    renderTexture.display();
    sf::Image image = renderTexture.getTexture().copyToImage();
    window.clear(sf::Color::Black);
    // Draw the RenderTexture onto the window
    sf::Sprite sprite(renderTexture.getTexture());
    window.draw(sprite);
    sf::Clock clock;
    float power = 0;
    float power1 = sensors(window, image, mousePosition, -180 + 35, 5, 5 * 10);
    float power2 = sensors(window, image, mousePosition, 90 - 5, 5, 5 * 20);
    s = std::to_string(clock.restart().asMilliseconds()) + "ms\n";
    sf::Vector2f hitPosition = castRay(renderTexture.getTexture().copyToImage(), mousePosition, 270);
    //    power = distance(mousePosition, hitPosition);
    s += "Mouse: " + std::to_string((int)mousePosition.x) + ", " + std::to_string((int)mousePosition.y) + "\n";
    s += "Hit: " + std::to_string((int)hitPosition.x) + ", " + std::to_string((int)hitPosition.y) + "\n";
    s += "sensor 1: " + std::to_string((int)(power1)) + "\n";
    s += "sensor 2: " + std::to_string((int)(power2)) + "\n";
    text.setString(s);
    text.setPosition(400, 50);
    window.draw(text);
    // Display the window
    window.display();
  }

  return 0;
}
