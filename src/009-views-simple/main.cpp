#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multi-samplings to use. 4 is probably fine
  sf::RenderWindow window(sf::VideoMode(800, 400), WINDOW_TITLE, sf::Style::Default, settings);
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
  sf::Vector2f mapSize(3000, 3000);

  /// define an object on the map
  sf::RectangleShape map_box({2800, 2800});
  map_box.setFillColor(sf::Color::Green);

  /// define an object in the UI
  sf::RectangleShape ui_box({100, 100});
  ui_box.setFillColor(sf::Color::Red);

  /// The map is all drawn way over to the right in
  /// pixel space and it is big. We will want it to retain
  /// its aspect ratio when the window resizes
  sf::View mapView(sf::FloatRect(1000, 0, 3000, 3000));
  /// we want it to occupy the right hal of the window
  mapView.setViewport(sf::FloatRect(0.5, 0, 0.5, 1.0));

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    /// make the map area fit in the space left after the UI
    sf::Vector2u windowSize = window.getSize();
    float map_left = 230.0f;
    sf::FloatRect screenRect(0, 0, static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    sf::View uiView(screenRect);
    float windowAspect = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    float mapAspect = mapSize.x / mapSize.y;
    if (windowAspect > mapAspect) {
      mapView.setSize(mapSize.x * windowAspect / mapAspect, mapSize.y);
    } else {
      mapView.setSize(mapSize.x, mapSize.y * mapAspect / windowAspect);
    }
    float frac = map_left / windowSize.x;
    mapView.setViewport(sf::FloatRect(frac, 0.0f, 1.0f - frac, 1.0f));

    //    float map_width = windowSize.x - map_left;  // Calculate the aspect ratio of the view
    //    float aspectRatio = map_width / windowSize.y;
    //    mapView.setSize(3000.0f, 3000.0f / aspectRatio);
    //    mapView.setCenter(1000.0 + 1500, 1500 / aspectRatio);
    //    // Update the viewport to ensure the map is 230 pixels from the left
    //    float viewportLeft = map_left / windowSize.x;
    //    mapView.setViewport(sf::FloatRect(viewportLeft, 0, 1 - viewportLeft, 1.0f));

    ui_box.setPosition(20, 20);
    map_box.setPosition(1100, 100);

    window.clear();

    window.setView(mapView);
    window.draw(map_box);

    window.setView(uiView);
    window.draw(ui_box);
    window.draw(text);
    window.display();
  }
  return 0;
}