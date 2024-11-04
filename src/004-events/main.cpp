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
  sf::RenderWindow window{sf::VideoMode(600, 400), WINDOW_TITLE, sf::Style::Default, settings};
  window.setFramerateLimit(60);

  sf::Clock up_time{};

  /// you need the absolute or relative path to the font file.
  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }
  sf::Text message("", font, 24);

  sf::Color bg = sf::Color::Black;
  sf::Color fg = sf::Color::White;
  int q = 0;
  int mx = 0;
  int my = 0;

  while (window.isOpen()) {
    /// process all the inputs
    /// There are lots of possible events. Here are some of the more likely ones
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
        case sf::Event::TextEntered: {
          if (event.text.unicode < 128) {
            std::cout << "ASCII: " << (char)event.text.unicode << std::endl;
          }
          break;
        }
        case sf::Event::KeyPressed: {
          if (event.key.scancode == sf::Keyboard::Scan::Up) {
            q++;
          } else if (event.key.scancode == sf::Keyboard::Scan::Down) {
            q--;
          }
          break;
        }
        case sf::Event::MouseMoved: {
          mx = event.mouseMove.x;
          my = event.mouseMove.y;
          break;
        }
        case sf::Event::MouseWheelScrolled: {
          q += (int)event.mouseWheelScroll.delta;
          break;
        }

        default:
          break;
      }
    }

    /// update the objects
    char buf[100];
    sprintf(buf, "window size: %dx%d - Q = %d @ (%d,%d)", window.getSize().x, window.getSize().y, q, mx, my);
    message.setString(buf);
    message.setFillColor(fg);

    bg = window.hasFocus() ? sf::Color(32, 32, 32) : sf::Color(0, 0, 0);

    /// and redraw the window
    window.clear(bg);
    window.draw(message);

    window.display();
  }

  return 0;
}
