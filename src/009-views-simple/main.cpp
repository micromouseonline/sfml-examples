#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include "drawing.h"

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
  sf::RenderWindow window(sf::VideoMode(1200, 800), WINDOW_TITLE, sf::Style::Close | sf::Style::Titlebar, settings);
  window.setFramerateLimit(60);

  /**
   * Simulate our working area. It is way off to one side, we can draw stuff in the UI on the left and
   * have a  window up to 5000 pixels wide without ever over-writing the map.
   * For the display though, we present them in a different order. Because we can.
   */
  float windowWidth = window.getSize().x;
  float windowHeight = window.getSize().y;
  float mapSize = 4000.0f;  // or whatever region we want. it will get scaled
  float mapOffset = 5000.0f;
  sf::View mapView;
  mapView.setSize(windowHeight, windowHeight);
  mapView.setCenter(mapOffset + mapSize / 2.0f, mapSize / 2.0f);

  // but we want the map to be displayed in a square region
  float mapFraction = (float)window.getSize().y / (float)window.getSize().x;
  mapView.setViewport(sf::FloatRect(0.0f, 0.0f, mapFraction, 1.0f));

  // Use the rest of the screen for the UI. We do not want is scaled
  sf::View uiView;
  float uiWidth = windowWidth - mapSize;
  uiView.setSize({uiWidth, windowHeight});
  uiView.setCenter(uiWidth / 2.0f, windowHeight / 2.0f);
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