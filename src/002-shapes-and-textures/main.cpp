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
  sf::RenderWindow window{sf::VideoMode(600, 400), WINDOW_TITLE, sf::Style::Default, settings};

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

  /// First let us load an image. This tileset contains a stack of 64x64 pixel images.
  /// Or, if you prefer, an array of 16x16px images in an array 4 wide by 32 high
  /// Or, an array of 64x64px images in an array 1x8.
  /// You get to choose.
  sf::Image tile_set;
  if (!tile_set.loadFromFile("./assets/images/tiles-64x64.png")) {
    std::cerr << "Unable to load spaceship images\n";
    exit(1);
  }

  /// Then use it to create the texture on the GPU. We copy the whole thing. It can be sliced
  /// up later by the GPU
  sf::Texture texture;
  texture.loadFromImage(tile_set);
  /// QUERY: can we now delete the image?

  /// Rectangles are shapes with a width and height
  /// They can have colours and textures as well as outlines
  /// Start with a simple square the same width as our tile set.
  sf::Vector2f floor_tile_size(64, 64);
  sf::RectangleShape floor_tile(floor_tile_size);
  floor_tile.setOrigin(floor_tile_size.x / 2, floor_tile_size.y / 2);
  floor_tile.setPosition(300, 200);
  /// and we can add a texture reference to the rectangle
  /// By default the entire texture will be scaled to fit the rectangle
  floor_tile.setTexture(&texture);
  /// So we map only a portion of the texture to pull out one image
  floor_tile.setTextureRect(sf::IntRect(0, 0, 64, 64));

  /// Now we will have another single tile from the same texture
  int grid_size = 16;
  sf::Vector2f static_tile_size(2 * grid_size, 2 * grid_size);
  sf::RectangleShape static_tile(static_tile_size);
  static_tile.setPosition(3 * grid_size, 2 * grid_size);

  static_tile.setTexture(&texture);
  int left = 1 * grid_size;
  int top = 5 * grid_size;
  int width = 2 * grid_size;
  int height = 2 * grid_size;
  static_tile.setTextureRect(sf::IntRect(left, top, width, height));

  sf::Clock deltaClock{};
  int top_row = 0;

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

    /// rotate the rect and its texture rotates with it.
    /// the graphics card handles all that.
    floor_tile.rotate(45 * time.asSeconds());

    /// lets also change the part of the texture we use
    /// just because we can
    if (++top_row > 16 * 12) {
      top_row = 0;
    }
    floor_tile.setTextureRect(sf::IntRect(0, top_row, 64, 64));

    /// and redraw the window
    window.clear();
    window.draw(floor_tile);
    window.draw(static_tile);
    window.display();
  }

  return 0;
}
