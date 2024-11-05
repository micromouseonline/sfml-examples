#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>

/***
 * This program just  displays the window's frame rate.
 *
 * Experiment with the frame rate setting just after opening the window to see the effect.
 * @return
 */
int main() {
  sf::RenderWindow window(sf::VideoMode(600, 100), "Frame Rate Example", sf::Style::Close);

  /**
   * The refresh can be set either as a fixed number of frames per second or
   * it can be tied to the vertical refresh.
   *
   * Manually setting a frame rate is not guaranteed to be synchronous with the display - it
   * just uses a timer internally.
   *
   * VSync is not guaranteed to be available and is not independent of the hardware so behaviour may
   * change from machine to machine or even monitor to monitor.
   *
   * You can set the frame rate to zero to just let everything run as fast as possible. The clock used
   * below gets you the elapsed time per frame so that you can do refresh-rate independent animation.
   * However, this can use a lot of processing power updating objects in a way that will not be seen
   * by the user. Also, the asynchronous screen updates can cause tearing if there is no VSync support
   * in the graphics hardware.
   *
   * You can enable VSync first _and_ a frame rate limit. If VSync is available it will be used
   * preferentially so long as the manually set frame rate is higher.
   *
   * That is to say, the actual frame rate for these two statements will be the lower of
   *    - the monitor frame rate
   *    - the manually set frame rate
   *
   * It is not clear what happens if you have two monitors running at difference refresh rates.
   */
  window.setFramerateLimit(100);
  window.setVerticalSyncEnabled(true);

  sf::Font font;
  /// This will generate an error message to stderr if it fails
  if (!font.loadFromFile("assets/fonts/consolas.ttf")) {
    exit(1);
  }

  sf::Text text("", font, 48);
  text.setFillColor(sf::Color::White);
  text.setPosition(10, 10);

  sf::Clock clock;
  int frame_count = 0;
  int frame_interval = 20;

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    frame_count++;
    float elapsed = clock.getElapsedTime().asSeconds();
    if (elapsed > 0.5f) {
      std::ostringstream ss;
      float frame_rate = static_cast<float>(frame_count) / elapsed;
      ss << "Frame Rate: " << std::fixed << std::setprecision(2) << frame_rate;
      text.setString(ss.str());
      frame_count = 0;
      clock.restart();
    }

    window.clear();
    window.draw(text);
    window.display();
  }

  return 0;
}
