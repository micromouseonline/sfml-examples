#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <string>

constexpr int WINDOW_WIDTH = 1440;
constexpr int WINDOW_HEIGHT = 980;
constexpr float MAP_DISPLAY_SIZE = 980.0f;
constexpr float MAP_OFFSET = 5000.0f;
constexpr float MAP_SCALE = 3.0f;

void fillViewWithColor(sf::RenderWindow& window, const sf::Color& color) {
  const sf::View view = window.getView();
  const sf::Vector2f viewSize = view.getSize();
  const sf::Vector2f viewCenter = view.getCenter();
  sf::RectangleShape rectangle(viewSize);
  rectangle.setFillColor(color);  // Set the position of the rectangle to the top-left corner of the view
  rectangle.setPosition(viewCenter - viewSize / 2.f);
  window.draw(rectangle);
}

sf::Vector2f GetRectCenter(const sf::FloatRect& rect) {
  return {rect.left + rect.width / 2.f, rect.top + rect.height / 2.f};
}

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multi-samplings to use. 4 is probably fine
  /// do not allow the window to resize
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Close | sf::Style::Titlebar, settings);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/arial.ttf")) {
    return -1;
  }
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(20);
  text.setString("User Interface Region");
  text.setPosition(20, 13);
  text.setFillColor(sf::Color::Yellow);

  /// Make a sprite that is the size of the map so we have something to display
  sf::Texture texture;
  if (!texture.loadFromFile("./assets/images/map-2892x2892.png")) {
    return -1;
  };

  sf::Sprite map;
  map.setTexture(texture);

  /**
   * The map is all drawn way over to the right in world space and it is big.
   *
   * With the map out of the way, we can draw stuff in the UI starting at the origin
   * and have a working area up to 5000 pixels wide without ever interfering with the map.
   */

  /// In this application, the map will be scaled by a factor of three
  /// you can also use the view.zoom() method
  sf::FloatRect mapRect(MAP_OFFSET, 0.0f, MAP_DISPLAY_SIZE * MAP_SCALE, MAP_DISPLAY_SIZE * MAP_SCALE);

  ///// Define the map view
  sf::View mapView;
  mapView.setSize(mapRect.width, mapRect.height);  /// in world pixel space
  /// the center is not just half the width because we offset it to the right
  mapView.setCenter(GetRectCenter(map.getLocalBounds()));
  // make sure this region is square
  float mapFraction = (float)window.getSize().y / (float)window.getSize().x;
  /// now restrict ourselves to the left part of the display window
  mapView.setViewport(sf::FloatRect(0.0f, 0.0f, mapFraction, 1.0f));

  /// Define the user interface view. use up all the rest of the width
  /// and take up the entire window height
  float uiWidth = (1.0f - mapFraction) * (float)window.getSize().x;

  sf::View uiView;
  uiView.setSize({uiWidth, (float)window.getSize().y});
  uiView.setCenter(uiWidth / 2.0f, window.getSize().y / 2);
  uiView.setViewport(sf::FloatRect(mapFraction, 0.0f, 1.0f - mapFraction, 1.0f));

  /// Finally we can get to business.
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();

    /// Draw the map in its view
    window.setView(mapView);
    window.draw(map);

    // draw the UI in its view
    window.setView(uiView);
    fillViewWithColor(window, sf::Color(255, 255, 255, 64));
    window.draw(text);

    window.display();
  }
  return 0;
}