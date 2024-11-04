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
  sf::Font aw_font;
  if (!aw_font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }

  sf::Text message("----", aw_font, 24);

  sf::Color bg = sf::Color::Black;
  sf::Color fg = sf::Color::White;
  int q = 0;
  int mx = 0;
  int my = 0;

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
        case sf::Event::TextEntered: {
          if (event.text.unicode < 128) {
            std::cout << "ASCII: " << (char)event.text.unicode << std::endl;
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

    // get the global mouse position (relative to the desktop)
    sf::Vector2i globalPosition = sf::Mouse::getPosition();

    // get the local mouse position (relative to a window)
    sf::Vector2i localPosition = sf::Mouse::getPosition(window);  // window is a sf::Window

    // Get the window's position and size
    sf::Vector2i windowPosition = window.getPosition();
    sf::Vector2u windowSize = window.getSize();
    // Create an sf::Rect object representing the window's position and size
    sf::IntRect windowRect(windowPosition.x, windowPosition.y, (int)windowSize.x, (int)windowSize.y);
    if (windowRect.contains(globalPosition)) {
      fg = sf::Color::Cyan;
    }
    /// You can test the state of the mouse and keyboard directly but you
    /// must check if the window has focus or it responds everywhere
    if (window.hasFocus()) {
      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        fg = sf::Color::Red;
      } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        fg = sf::Color::Green;
      }
    }

    /// update the objects
    char buf[100];
    sprintf(buf, "Mouse Position:\n Global: (%d,%d) \n Local: (%d,%d) \n Buttons: %d", globalPosition.x, globalPosition.y, localPosition.x, localPosition.y,
            sf::Mouse::isButtonPressed(sf::Mouse::Left));
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
