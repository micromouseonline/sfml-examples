#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(600, 400), "006 Keyboard", sf::Style::Default, settings};
  window.setFramerateLimit(60);

  sf::Clock up_time{};

  /// you need the absolute or relative path to the font file.
  sf::Font aw_font;
  if (!aw_font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }

  sf::Text message("----", aw_font, 24);

  sf::Color bg = sf::Color::Black;
  sf::Color fg = sf::Color::White;
  float dx = 12.5;
  float dy = 12.5;
  float x = 0;
  float y = 0;
  float scale = 1.0f;

  sf::RectangleShape rectangle(sf::Vector2f(100, 100));
  rectangle.setFillColor(sf::Color(132, 132, 132, 96));
  while (window.isOpen()) {
    /// process all the inputs
    sf::Event event{};
    fg = sf::Color::White;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed: {
          window.close();
          break;
        }
        case sf::Event::Resized: {
          sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
          window.setView(sf::View(visibleArea));  // or everything distorts
          break;
        }
        default:
          break;
      }
    }

    /// Test the key presses OUTSIDE the event loop for better responsiveness
    /// BUT - this is the global state of the keyboard so make sure we have focus
    if (window.hasFocus()) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        scale = 0.1f;
      } else {
        scale = 1.0f;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        y -= dy * scale;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        y += dy * scale;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        x -= dx * scale;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        x += dx * scale;
      }

      x = std::max(0.0f, std::min(x, (float)window.getSize().x - rectangle.getSize().x));
      y = std::max(0.0f, std::min(y, (float)window.getSize().y - rectangle.getSize().y));
    }

    /// update the objects
    char buf[100];
    sprintf(buf, "Rectangle Position:(%d,%d)", (int)x, (int)y);
    message.setString(buf);
    message.setFillColor(fg);
    rectangle.setPosition(x, y);

    /// and redraw the window
    window.clear(bg);
    window.setTitle(buf);
    window.draw(message);
    window.draw(rectangle);

    window.display();
  }

  return 0;
}
