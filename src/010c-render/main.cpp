#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>

/***
 * Minimal example to demonstrate how to
 *  - render to a texture,
 *  - display the texture,
 *  - create a corresponding image
 *  - examine the pixels in that image
 *
 * With SFML, it is not possibe to query the pixels in a texture directly.
 * Instead, we have to render the texture to an image and then examine the pixels in that image
 *
 * A rectangular portion of the texture can be copid to an mage in RAM and then queried
 * using the getPixel() method.
 *
 * This makes pixel-perfect colliion detection possible but relatively expensive if the map is large.
 *
 * If the texture is relatively static then the local image copy need only be updated when the texture
 * is updated rather than every frame. SFML has no function that will do this for us and copying
 * pixel-by-pixel is slow.
 *
 * It is much faster to copy the entire texture even though such a structure is too big
 * to fit in the processor cache and give us fast execution
 *
 */

bool isWithinBounds(const sf::Vector2u& size, int x, int y) {
  return x >= 0 && x < static_cast<int>(size.x) && y >= 0 && y < static_cast<int>(size.y);
}

sf::Color getColorAtPixel(const sf::Image& image, int x, int y) {
  return isWithinBounds(image.getSize(), x, y) ? image.getPixel(x, y) : sf::Color::Transparent;
}

int main() {
  // Create the window
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;                      // the number of multisamplings to use. 4 is probably fine
  int style = sf::Style::Titlebar | sf::Style::Close;  //!< Default window style
  sf::RenderWindow window(sf::VideoMode(1200, 800), WINDOW_TITLE, style, settings);

  /// load your resources early in case they fail
  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }
  /// Create a RenderTexture that we can draw on
  /// it behaves exactly like the RenderWindow but lives in memory (GPU?)
  sf::RenderTexture map_texture;
  if (!map_texture.create(800, 600)) {
    // Error handling
    return -1;
  }

  sf::Cursor cursor;
  cursor.loadFromSystem(sf::Cursor::Cross);
  window.setMouseCursor(cursor);

  /// create some renderable objects
  sf::RectangleShape post(sf::Vector2f(12, 12));
  post.setFillColor(sf::Color::Red);
  post.setPosition(150, 150);

  sf::RectangleShape wall(sf::Vector2f(166, 12));
  wall.setFillColor(sf::Color::Red);
  wall.setPosition(163, 150);

  /// add the texture to a sprite for display
  /// changes to te rendertexture are automatically passed through
  sf::Sprite map(map_texture.getTexture());

  /// Clear the map texture
  map_texture.clear(sf::Color(32, 32, 32));
  /// Draw some objects
  map_texture.draw(post);
  map_texture.draw(wall);
  post.move(180, 180);
  map_texture.draw(post);  /// interesting - we can draw the same object many times
  map_texture.display();   /// always finalise

  /// We can store the texture as an image in RAM
  /// but if the texture changes we must do the copy again
  sf::Image image = map_texture.getTexture().copyToImage();

  /// Text uses the sf::Text class
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(24);                       // in pixels, not points!
  text.setFillColor(sf::Color(255, 255, 0, 255));  // it can be any colour
  text.setPosition(810, 10);

  int64_t time = 100;
  int fps = 0;
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

    sf::Clock clock;
    // Clear the window
    window.clear(sf::Color(0, 0, 32));
    // Draw the RenderTexture onto the window
    window.draw(map);  /// this is very quick
    fps = 1000000 / time;
    s = "       Time: " + std::to_string(time) + " us (" + std::to_string(fps) + " fps) \n";
    s += "     Mouse: " + std::to_string((int)mousePosition.x) + ", " + std::to_string((int)mousePosition.y) + "\n";
    s += "Map Colour: ";
    if (isWithinBounds(image.getSize(), mousePosition.x, mousePosition.y)) {
      sf::Color map_colour = getColorAtPixel(image, mousePosition.x, mousePosition.y);
      s += std::to_string((int)map_colour.r) + ", ";
      s += std::to_string((int)map_colour.g) + ", ";
      s += std::to_string((int)map_colour.b) + ", ";
      s += std::to_string((int)map_colour.a) + "\n";
    }
    text.setString(s);
    window.draw(text);
    /// low pas filter the time for a more stable view
    time = (10 * clock.restart().asMicroseconds() + 990 * time) / 1000;
    // Display the window
    window.display();
  }

  return 0;
}
