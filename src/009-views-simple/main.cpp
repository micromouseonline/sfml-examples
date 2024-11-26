#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include "drawing.h"

void draw_axes(sf::RenderWindow& window) {
  /// draw the x and y axes so that we can retain some idea where we are
  sf::CircleShape origin(10);
  origin.setFillColor(sf::Color::Transparent);
  origin.setOutlineColor(sf::Color::Red);
  origin.setOutlineThickness(4);
  origin.setPosition(0, 0);
  origin.setOrigin(10, 10);
  window.draw(origin);
  draw_line(window, {0, 0}, {10000, 0}, sf::Color::Green);
  draw_line(window, {0, 0}, {0, 10000}, sf::Color::Yellow);
}

void fillViewWithColor(sf::RenderWindow& window, const sf::Color& color) {
  sf::View view = window.getView();
  sf::Vector2f viewSize = view.getSize();
  sf::Vector2f viewCenter = view.getCenter();
  sf::RectangleShape rectangle(viewSize);
  rectangle.setFillColor(color);  // Set the position of the rectangle to the top-left corner of the view
  rectangle.setPosition(viewCenter - viewSize / 2.f);
  window.draw(rectangle);
}

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multi-samplings to use. 4 is probably fine
  /// do not allow the window to resize
  sf::RenderWindow window(sf::VideoMode(980 + 400, 980), WINDOW_TITLE, sf::Style::Close | sf::Style::Titlebar, settings);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    return -1;
  }
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(12);
  text.setString("User Interface");
  text.setPosition(20, 3);

  /**
   * Simulate our working area. The map is all drawn way over to the right in
   * world space and it is big.
   *
   * with the map out of the way, we can draw stuff in the UI on the left and
   * have a window up to 5000 pixels wide without ever seeing the map.
   */
  sf::Image image;
  image.loadFromFile("./assets/images/map-2892x2892.png");
  sf::Texture texture;
  texture.loadFromImage(image);
  sf::Sprite sprite;
  sprite.setTexture(texture);
  sprite.setPosition(5000, 0);
  sf::Vector2f mapSize(image.getSize().x, image.getSize().y);

  sf::View mapView;
  mapView.setSize(mapSize.x, mapSize.y);
  mapView.setCenter(sprite.getPosition().x + mapSize.x / 2, mapSize.y / 2);
  // but we want the map to be displayed in a square region at the
  // top left of the window.
  float frac = (float)window.getSize().y / (float)window.getSize().x;
  mapView.setViewport(sf::FloatRect(0.0f, 0.0f, frac, 1.0f));

  /// The UI will be drawn using the default view and positioned to the right of the map
  float uiLeftEdge = window.getSize().x - 400;
  /// or we could use
  // float uiLeftEdge = window.getSize().x - window.getSize().y;

  /// define an object in the UI
  sf::RectangleShape ui_box({400, (float)window.getSize().y});
  ui_box.setFillColor(sf::Color(127, 128, 0, 255));
  ui_box.setPosition(0, 0);

  // there is no need for a view or viewport but...
  sf::View uiView;
  uiView.setSize({400, (float)window.getSize().y});
  uiView.setCenter(200, window.getSize().y / 2);
  uiView.setViewport(sf::FloatRect(frac, 0.0f, 1.0f - frac, 1.0f));

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();
    window.setView(mapView);
    window.draw(sprite);
    // now we can revert to the default view and draw directly
    // fillViewWithColor(window, sf::Color::Green);
    // window.setView(window.getDefaultView());
    window.setView(uiView);
    // fillViewWithColor(window, sf::Color::Blue);
    draw_axes(window);
    window.draw(ui_box);

    window.display();
  }
  return 0;
}