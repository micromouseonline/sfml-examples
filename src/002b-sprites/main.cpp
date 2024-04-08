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
  sf::RenderWindow window{sf::VideoMode(600, 400), "002-shapes-and-textures", sf::Style::Default, settings};
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
  sf::Texture space_things;
  if (!space_things.loadFromFile("./assets/images/spaceship.png")) {
    std::cerr << "Unable to load spaceship textures\n";
    exit(1);
  }

  /// Rectangles are shapes with a width and height
  /// They can have colours and textures as well as outlines
  sf::Sprite ship(space_things);
  sf::Rect rect_left(0, 40, 48, 36);
  sf::Rect rect_main(48, 0, 48, 36);
  sf::Rect rect_right(96, 40, 48, 36);
  ship.setTextureRect(rect_main);
  ship.setOrigin(rect_main.width / 2.0f, rect_main.height / 2.0f);
  ship.setPosition(visibleArea.width / 2, visibleArea.height / 2);
  ship.scale(1, 1);

  /// Sprites are simpler and _require_ a texture. They are the drawable
  /// representation of a texture
  /// We will load it direct this time.
  sf::Texture chicken;
  if (!chicken.loadFromFile("./assets/images/chicken.png")) {
    std::cerr << "Unable to load chicken texture\n";
    exit(1);
  }

  sf::Clock deltaClock{};
  int state = 0;
  bool key_down = false;
  float vx = 0;
  float vmax = 150.0;  // pixels per second

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
        ship.setPosition(visibleArea.getSize().x / 2, visibleArea.getSize().y / 2);
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
      ship.setTextureRect(rect_left);
      ship.setOrigin(rect_left.width / 2.0f, rect_left.height / 2.0f);
      vx = -vmax;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
      ship.setTextureRect(rect_right);
      ship.setOrigin(rect_right.width / 2.0f, rect_right.height / 2.0f);
      vx = vmax;
    } else {
      ship.setTextureRect(rect_main);
      ship.setOrigin(rect_main.width / 2.0f, rect_main.height / 2.0f);
      vx = 0;
    }

    /// update the objects
    sf::Time dt = deltaClock.restart();

    /// rotate the rect and its texture rotates with it.
    /// the graphics card handles all that.
    //    ship.rotate(45 * time.asSeconds());
    float deltax = vx * dt.asSeconds();
    if ((ship.getPosition().x + deltax > 24) && ((ship.getPosition().x + deltax) < (visibleArea.width - 24))) {
      ship.move(deltax, 0);
    }
    /// and redraw the window
    window.clear();
    window.draw(ship);
    window.display();
  }

  return 0;
}
