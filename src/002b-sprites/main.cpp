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
  sf::Image space_things;
  if (!space_things.loadFromFile("./assets/images/spaceship.png")) {
    std::cerr << "Unable to load spaceship images\n";
    exit(1);
  }
  /// Then use it to create the texture on the GPU
  sf::Texture texture;
  texture.loadFromImage(space_things);

  /// Rectangles are shapes with a width and height
  /// They can have colours and textures as well as outlines
  sf::Vector2f size(55, 53);
  sf::RectangleShape rect(size);
  rect.setOrigin(size.x / 2, size.y / 2);
  rect.setPosition(300, 200);
  /// and we can add a texture reference to the rectangle
  /// By default the entire texture will be scaled to fit the rectangle
  rect.setTexture(&texture);
  /// So we map only a portion of the texture to pull out one image
  rect.setTextureRect(sf::IntRect(39, 0, 40, 36));
  rect.setTextureRect(sf::IntRect(0, 153, 55, 53));
  rect.scale(2, 3);

  /// Sprites are simpler and _require_ a texture. They are the drawable
  /// representation of a texture
  /// We will load it direct this time.
  sf::Texture chicken;
  if (!chicken.loadFromFile("./assets/images/chicken.png")) {
    std::cerr << "Unable to load chicken texture\n";
    exit(1);
  }

  /// The chicken will be animated by switching the texture rectangle in
  /// a predefined sequence
  const int seq[] = {1, 2, 1, 0};
  sf::Sprite chick;
  chick.setTexture(chicken);
  chick.setTextureRect(sf::IntRect(0, 0, 32, 32));
  chick.setOrigin(16, 16);
  chick.setPosition(180, 200);
  chick.scale(2, 2);
  chick.setColor(sf::Color(255, 255, 0, 255));

  sf::Clock deltaClock{};
  int state = 0;

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
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        /// Note that constructing a view with a sf::Rect defines the corners
        /// Using two points defines the _centre_ and the width,height
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }

    /// update the objects
    sf::Time time = deltaClock.restart();
    chick.rotate(90 * time.asSeconds());  /// 90 degrees per second

    float angle = chick.getRotation();
    float dx = std::cos((angle - 90) * 3.14 / 180) * 200;
    float dy = std::sin((angle - 90) * 3.14 / 180) * 200;
    chick.move(sf::Vector2f(dx, dy) * time.asSeconds());
    /// calculate the next animation frame location
    chick.setTextureRect(sf::IntRect(32 * seq[state / 5], 0, 32, 32));
    state = (++state) % 20;

    /// rotate the rect and its texture rotates with it.
    /// the graphics card handles all that.
    rect.rotate(45 * time.asSeconds());

    /// and redraw the window
    window.clear();
    window.draw(rect);
    window.draw(chick);
    window.display();
  }

  return 0;
}
