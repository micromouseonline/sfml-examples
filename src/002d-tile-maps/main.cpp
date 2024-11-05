#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

/***
 * Create a simple tile map using sprites.
 *
 * It is more efficient to use a vertex array. See the other example
 */

const int TILE_SIZE = 16;  // Assuming each tile is 32x32 pixels

// Sample map (2D array of integers)
// clang-format off
std::vector<std::vector<int>> map = {
    { 0,  1,  2,  3,  33},
    { 4,  5,  6,  7,  37},
    { 8,  9, 10, 11, 120},
    {12, 13, 14, 15, 124},
    {16, 17, 18, 19,  43},
    {20, 21, 22, 23,  47},
    {24, 25, 26, 27,  64},
    {28, 29, 30, 32,  68}
  };
// clang-format on

int main() {  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(600, 400), WINDOW_TITLE, sf::Style::Default, settings};

  window.setVerticalSyncEnabled(true);

  sf::Texture texture;
  texture.loadFromFile("./assets/images/tiles-64x64.png");

  std::vector<sf::Sprite> tiles;

  // create a simple vector of sprites and their positions
  for (size_t i = 0; i < map.size(); ++i) {
    for (size_t j = 0; j < map[i].size(); ++j) {
      int tile_index = map[i][j];
      int tile_x = (tile_index % 4) * TILE_SIZE;
      int tile_y = (tile_index / 4) * TILE_SIZE;
      sf::Sprite sprite(texture);
      sprite.setTextureRect(sf::IntRect(tile_x, tile_y, TILE_SIZE, TILE_SIZE));
      sprite.setPosition(j * TILE_SIZE, i * TILE_SIZE);
      tiles.push_back(sprite);
    }
  }

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
      // Maintain aspect ratio
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }

    /// update the objects
    sf::Time time = deltaClock.restart();

    /// and redraw the window
    window.clear();

    for (const auto& tile : tiles) {
      window.draw(tile);
    }
    window.display();
  }

  return 0;
}
