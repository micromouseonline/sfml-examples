#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include "drawing.h"

constexpr int ANTIALIASING_LEVEL = 8;
constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;
constexpr int FRAME_RATE = 60;
constexpr float MAP_SIZE = 4000.0f;
constexpr float MAP_OFFSET = 5000.0f;

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
  settings.antialiasingLevel = ANTIALIASING_LEVEL;  // the number of multi-samplings to use. 4 is probably fine
  /// do not allow the window to resize
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Close | sf::Style::Titlebar, settings);
  window.setFramerateLimit(FRAME_RATE);

  /**
   * Simulate our working area. It is way off to one side, we can draw stuff in the UI on the left and
   * have a  window up to 5000 pixels wide without ever over-writing the map.
   * For the display though, we present them in a different order. Because we can.
   */
  float windowWidth = window.getSize().x;
  float windowHeight = window.getSize().y;
  float mapSize = MAP_SIZE;  // or whatever region we want. it will get scaled
  float mapOffset = MAP_OFFSET;
  /// we will need a view of the map
  sf::View mapView;
  mapView.setSize(windowHeight, windowHeight);
  /// remember the offset when finding the center
  mapView.setCenter(mapOffset + mapSize / 2.0f, mapSize / 2.0f);

  // e want the map to be displayed in a square region of the window
  // let's put it on the left
  float mapFraction = (float)window.getSize().y / (float)window.getSize().x;
  // viewports are always defined in terms of fractions of the window size
  mapView.setViewport(sf::FloatRect(0.0f, 0.0f, mapFraction, 1.0f));

  // Use the rest of the screen for the UI. We do not want it scaled
  sf::View uiView;
  float uiWidth = windowWidth - mapSize;
  sf::Vector2f uiSize(uiWidth, windowHeight);
  // the size is in WORLD coordinates
  uiView.setSize(uiSize);
  // positioning of views is always with reference to their centre
  // in WORLD coordinates - think of it as the 'camera' aim point
  uiView.setCenter(uiWidth / 2.0f, windowHeight / 2.0f);
  // now use the rest of the screen for the viewport
  uiView.setViewport(sf::FloatRect(mapFraction, 0.0f, 1.0f - mapFraction, 1.0f));

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();
    window.setView(mapView);
    fillViewWithColor(window, sf::Color(0, 255, 0, 32));

    window.setView(uiView);
    fillViewWithColor(window, sf::Color(0, 0, 255, 32));

    /// we can still us the default view to draw in the window coordinates
    window.setView(window.getDefaultView());
    draw_line(window, {0, 0}, {800, 800});
    draw_line(window, {800, 0}, {00, 800});
    draw_line(window, {800, 0}, {1200, 800});
    draw_line(window, {1200, 0}, {800, 800});

    window.display();
  }
  return 0;
}