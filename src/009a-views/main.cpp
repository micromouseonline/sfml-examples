#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include <string>

/**
 * Using SFML, you can draw to an arbitrarily sized space - the render target.
 * Render targets may be sf::RenderWindow (you can create multiple windows)
 * or sf::RenderTexture (you can create multiple RenderTextures).
 *
 * While these can be arbitrarily large, there may be constraints due to
 * limited GPU memory and the amount of processing needed.
 *
 * For an application using only one window, that will be the default
 * render target. You may create that window at a size to suit the display,
 * but you are not constrained to draw objects inside that space.
 *
 * By defining a sf::View, you are defining a portion of the render target
 * that will be displayed in the window. That portion will be scaled to
 * fill the available space.
 *
 * If the view size and the render target size are not the same, the GPU
 * will simply scale the rendered content. Since this is a bitmap stored in
 * the GPU memory, there are likely to be aliasing or pixelation artifacts.
 */

/**
 * Draw a grid onto the supplied render target and label each cell
 * with its x,y coordinates.
 * Return the grid's global bounds
 *
 * It is not important what we draw for this demo. It is just some content
 * that makes it easy to understand which part we are looking at
 */

void shrink(sf::RectangleShape& rect, float pixels) {
  // Get the current size and position
  sf::Vector2f currentSize = rect.getSize();
  sf::Vector2f currentPos = rect.getPosition();
  sf::Vector2f newSize = currentSize - sf::Vector2f(pixels, pixels);
  rect.setSize(newSize);
  sf::Vector2f offset(pixels / 2, pixels / 2);
  rect.setPosition(currentPos + offset);
}

sf::FloatRect drawGrid(sf::RenderTarget& target, int cell_count, float cell_size, float wall_size, const sf::Font& font) {
  /// make and fill a background rectangle
  float size = cell_count * cell_size + wall_size;
  sf::RectangleShape bg(sf::Vector2f(size, size));
  bg.setFillColor(sf::Color::Cyan);
  target.draw(bg);
  shrink(bg, wall_size);
  bg.setFillColor(sf::Color(0, 0, 64));
  target.draw(bg);

  /// create a rectangle for the cell inside
  sf::RectangleShape cell(sf::Vector2f(cell_size - wall_size, cell_size - wall_size));
  cell.setFillColor(sf::Color::Transparent);
  cell.setOutlineColor(sf::Color::Cyan);
  /// Each cell 'owns' half a wall
  cell.setOutlineThickness(wall_size / 2);

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(static_cast<int>(cell_size / 4));
  text.setFillColor(sf::Color::White);

  for (int i = 0; i < cell_count; ++i) {
    for (int j = 0; j < cell_count; ++j) {
      float x = i * cell_size;
      float y = cell_count * cell_size - j * cell_size;
      cell.setOrigin(0, cell_size);
      cell.setPosition(x + wall_size, y + wall_size);
      target.draw(cell);

      text.setString("(" + std::to_string(i) + "," + std::to_string(j) + ")");
      float text_width = text.getGlobalBounds().width;
      float text_height = text.getGlobalBounds().height;
      text.setPosition(x + (cell_size - text_width) / 2, (y - (cell_size + text_height) / 2));
      target.draw(text);
    }
  }
  return bg.getGlobalBounds();
}

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multi-samplings to use. 4 is probably fine
  sf::RenderWindow window(sf::VideoMode(800, 800), "View Example", sf::Style::Default, settings);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    return -1;
  }

  /// Work out how big the grid will be. Assume 1 pixel == 1 mm
  float wall_size = 12.0f;
  float cell_size = 180.0f;
  int grid_width = 16;
  float grid_size = grid_width * cell_size + wall_size;

  /// create a render texture that the grid will be drawn onto
  /// attempts to draw outside this texture will be cropped
  sf::RenderTexture grid_texture;
  grid_texture.create(grid_size, grid_size);

  /// The grid view is static so we can just draw it once here
  grid_texture.clear();
  drawGrid(grid_texture, grid_width, cell_size, wall_size, font);
  grid_texture.display();

  /// And assign it to a sprite
  sf::Sprite grid(grid_texture.getTexture());

  /// now make a view big enough for only part of the scene
  /// Only that part of the scene will be displayed and it
  /// will fill the window
  float view_size = cell_size * 6;
  sf::View view(sf::FloatRect(0, 0, view_size, view_size));
  view.setCenter(sf::Vector2f(grid_texture.getSize().x / 2, grid_texture.getSize().y / 2));
  window.setView(view);
  sf::Clock deltaClock{};  /// Keeps track of elapsed time
  int elapsed = 0;
  int count = 0;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // Zoom view with mouse wheel
      if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
          view.zoom(15.0f / 16.0f);  // Zoom in
        } else {
          view.zoom(16.0 / 15.0);  // Zoom out
        }
        window.setView(view);
      }

      // move the view with arrow keys
      if (event.type == sf::Event::KeyPressed) {
        float delta = wall_size;
        if (event.key.code == sf::Keyboard::Left)
          view.move(-delta, 0);
        if (event.key.code == sf::Keyboard::Right)
          view.move(delta, 0);
        if (event.key.code == sf::Keyboard::Up)
          view.move(0, -delta);
        if (event.key.code == sf::Keyboard::Down)
          view.move(0, delta);
        window.setView(view);
      }
    }
    window.clear();
    deltaClock.restart();
    window.draw(grid);  // 10us
                        //    drawGrid(window, grid_width, cell_size, wall_size, font); // 5ms
    elapsed = deltaClock.restart().asMicroseconds();
    window.display();
    if (++count > 60) {
      count = 0;
      std::cout << elapsed << std::endl;
    }
  }

  return 0;
}
