#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

/***
 * Create a simple SFML window with custom title text and a couple of shapes drawn in it
 */

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(1200, 400), WINDOW_TITLE, sf::Style::Default, settings};
  window.setVerticalSyncEnabled(true);
  sf::FloatRect visibleArea(0, 0, (float)window.getSize().x, (float)window.getSize().y);

  /***
   * Textures are graphical elements that get loaded into the graphic card memory. Once there, they can
   * be rendered very quickly but are no longer directly available to the program. Instead, shapes and
   * sprites are told where sections of the texture are to be mapped from.
   *
   * Textures can be loaded directly into the GPU. If you have many elements, like animations for
   * example, place them all in one file and load the entire thing as one texture.
   *
   * If you want access to the original image, you should first load it as an image and then create a
   * texture from that image.
   */
  /// First let us load an image:
  sf::Texture nixie_digits;
  if (!nixie_digits.loadFromFile("./assets/images/nixie_red.png")) {
    exit(1);
  }
  int width = 135;
  int height = 280;
  int digits = 8;
  std::vector<sf::Sprite> sprites;
  for (int i = 0; i < digits; i++) {
    sf::Sprite sprite(nixie_digits);
    int n = rand() % 10;
    // sprite.setScale(0.25, 0.25);
    sprite.setTextureRect({(width + 10) * n + 1, 50, width, height - 20});
    sprite.setTextureRect({10 + 145 * n, 50, width, 250});
    sprite.setPosition(i * width, 25);
    sprites.push_back(sprite);
  }

  sf::Clock deltaClock{};
  uint16_t v = 0;
  /// this is the 'game loop'
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
        visibleArea.width = (float)event.size.width;
        visibleArea.height = (float)event.size.height;
        /// Note that constructing a view with a sf::Rect defines the corners
        /// Using two points defines the _centre_ and the width,height
        window.setView(sf::View(visibleArea));  /// or everything distorts
      }
    }

    /// update the objects
    sf::Time dt = deltaClock.restart();

    /// and redraw the window
    window.clear();
    for (int i = 0; i < digits; i++) {
      window.draw(sprites[i]);
    }
    window.display();
  }

  return 0;
}
