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
  if (!aw_font.loadFromFile("./assets/fonts/audiowide.ttf")) {
    exit(1);
  }

  sf::Font dm_font;
  if (!dm_font.loadFromFile("./assets/fonts/digital-display.ttf")) {
    exit(1);
  }

  /// add a rectangle to illustrate positioning
  sf::RectangleShape rect(sf::Vector2f(100, 100));
  rect.setFillColor(sf::Color(32, 32, 32));

  /// Text uses the sf::Text class
  sf::Text text;
  text.setString("Here is some text");
  text.setFont(aw_font);
  text.setCharacterSize(48);                             // in pixels, not points!
  text.setFillColor(sf::Color(255, 255, 0, 255));        // it can be any colour
  text.setStyle(sf::Text::Bold | sf::Text::Underlined);  // and have the usual styles
  text.setPosition(100, 100);

  /// text objects can be copied completely
  sf::Text v_text = text;
  v_text.setFillColor(sf::Color(255, 255, 0, 128));
  v_text.move(3, 3);

  /// Text objects can be rotated
  sf::Text r_text = text;
  r_text.setString("Rotate me");
  r_text.setCharacterSize(16);
  r_text.setStyle(sf::Text::Regular);
  r_text.setOrigin(r_text.getLocalBounds().width / 2, r_text.getLocalBounds().height / 2);
  r_text.rotate(-90);
  r_text.setPosition(100 + r_text.getLocalBounds().height / 2, 50);

  /// Make a seven segment display
  sf::Text time_text("88:88.888", dm_font, 96);
  time_text.setFillColor(sf::Color(0, 255, 0, 255));
  time_text.setPosition(150, 0);
  /// emulate the unlit background segments
  sf::Text segments = time_text;
  segments.setFillColor(sf::Color(60, 255, 60, 24));

  while (window.isOpen()) {
    /// process all the inputs
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      /// if you want items to stay the same size, you must look after resize events
      /// it can still look scrappy during the resize
      /// This works because the scene is effectively infinite and the window is just
      /// a view of it starting at (0,0)
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }

    /// update the objects
    char buf[20];
    float seconds = up_time.getElapsedTime().asSeconds();
    int mins = ((int)seconds / 60) % 60;
    sprintf(buf, "%2d:%06.3f", mins, fmodf(seconds, 60));
    time_text.setString(buf);

    /// and redraw the window
    window.clear();
    window.draw(rect);
    window.draw(v_text);
    window.draw(text);
    window.draw(r_text);
    window.draw(segments);
    window.draw(time_text);
    window.display();
  }

  return 0;
}
