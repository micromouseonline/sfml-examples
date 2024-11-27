#include <iostream>

#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"

/***
 * Create a simple SFML window with custom title text and a couple of shapes drawn in it
 *
 */

const float WINDOW_WIDTH = 800;
const float WINDOW_HEIGHT = 600;
int main() {
  // Get the desktop mode (resolution of the primary display)
  sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

  // Print the width and height of the primary display
  std::cout << "Display Width: " << desktopMode.width << std::endl;
  std::cout << "Display Height: " << desktopMode.height << std::endl;

  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine

  /**
   * When the window is instantiated, it will automatically be displayed. Where that is will
   * depend on the operating system. Changes to the position always
   * cause a jump because the window will always be created in a default location first
   */
  sf::RenderWindow window{sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Default, settings};

  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);

  /// You can set the window to a position in the monitor reference frame but this is likely not what you think
  /// The monitor reference frame depends on the operating system.
  /// On Windows, the window often opens centered or in the top-left corner of the primary display.
  /// On macOS or Linux, the behavior depends on the desktop environment or window manager.
  /// It may be centered, positioned based on the previous window location, or placed according to the
  /// operating system's default rules.

  // window.setPosition(sf::Vector2i(20, 20));
  // window.display();  /// call this early to ensure the window is positioned correctly. It will jump

  sf::Vector2i position = window.getPosition();
  std::cout << "Window Position: (" << position.x << ", " << position.y << ")\n";

  static float circleRadius = 40.f;
  sf::CircleShape circle{circleRadius};
  circle.setFillColor(sf::Color::Green);
  circle.setOrigin(circleRadius, circleRadius);
  circle.setPosition(100, 100);

  sf::Clock deltaClock{};  /// Keeps track of elapsed time

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      /// This is how you listen for the window close button being pressed
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      /// this is how you listen for a specific key being pressed
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scancode::Escape) {
          window.close();
        }
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

    /// Only after updating everything should you redraw the window.
    /// Just do the drawing in this section.
    /// Drawing is done to an off screen buffer in the GPU.
    /// always start from a clean slate
    window.clear();
    window.draw(circle);

    /// This makes the off-screen buffer live and will wait for the refresh event
    /// specified above. Normally a simple wait fo elapsed time is used
    window.display();
  }

  return 0;
}
