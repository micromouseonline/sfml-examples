#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include "animation.h"
#include "entity.h"
#include "player.h"
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
  /// The spritesheet holds all the required textures
  sf::Texture chicken_texture;
  if (!chicken_texture.loadFromFile("./assets/images/chicken.png")) {
    std::cerr << "Unable to load spaceship textures\n";
    exit(1);
  }
  chicken_texture.setSmooth(true);

  /// then we can specify the region of the spritesheet to be used.
  Animation player_main_animation(chicken_texture, 0, 0, 32, 32, 4, 6);
  Animation player_left_animation(chicken_texture, 0, 96, 32, 32, 4, 6);
  Animation player_right_animation(chicken_texture, 0, 32, 32, 32, 4, 6);

  Player player(visibleArea.width, visibleArea.height, player_main_animation, visibleArea.width / 2, visibleArea.height / 2, 0, 20);
  player.setName(PLAYER);

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
        player.setPosition(visibleArea.width / 2, visibleArea.height / 2);
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
      player.setAnimation(player_left_animation);
      player.setAngle(-90);
      player.setThrust(true);

      vx = -vmax;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
      player.setAnimation(player_right_animation);
      player.setAngle(90);
      player.setThrust(true);
      vx = vmax;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)) {
      player.setAnimation(player_main_animation);
      player.setAngle(0);
      player.setThrust(true);
      vx = 0;
    }

    /// update the objects
    sf::Time dt = deltaClock.restart();

    /// rotate the rect and its texture rotates with it.
    /// the graphics card handles all that.
    float deltax = vx * dt.asSeconds();

    player.update(dt.asSeconds());

    /// and redraw the window
    window.clear();
    player.draw(window);
    window.display();
  }

  return 0;
}
