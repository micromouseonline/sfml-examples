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
//////////////////////////////////////////////////////////////////////////
/**
 * Draw a grid onto the supplied render target (window) and label each cell
 * with its x,y coordinates.
 * Return the grid's global bounds
 *
 * This is an expensive way to do this in computational terms and will take
 * up quite a lot of GPU and CPU cycles.
 *
 * Since the scene is static, it would be better to draw it once onto
 * a RenderTexture and then display that - probably as a single sprite.
 */

sf::FloatRect drawGrid(sf::RenderWindow& window, int units, float unitSize, const sf::Font& font) {
  float size = units * unitSize;
  sf::RectangleShape bg(sf::Vector2f(size, size));
  bg.setFillColor(sf::Color(0, 0, 64));
  window.draw(bg);

  sf::RectangleShape cell(sf::Vector2f(unitSize, unitSize));
  cell.setFillColor(sf::Color::Transparent);
  cell.setOutlineColor(sf::Color::Cyan);
  cell.setOutlineThickness(unitSize / 16.0f);

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(static_cast<int>(unitSize / 4));
  text.setFillColor(sf::Color::White);

  for (int i = 0; i < units; ++i) {
    for (int j = 0; j < units; ++j) {
      float x = i * unitSize;
      float y = units * unitSize - j * unitSize;
      cell.setOrigin(0, unitSize);
      cell.setPosition(x, y);
      window.draw(cell);

      text.setString("(" + std::to_string(i) + "," + std::to_string(j) + ")");
      float text_width = text.getGlobalBounds().width;
      float text_height = text.getGlobalBounds().height;
      text.setPosition(x + (unitSize - text_width) / 2, (y - (unitSize + text_height) / 2));
      window.draw(text);
    }
  }
  return bg.getGlobalBounds();
}

/////////////////////////////////////////////////////////////////////////////////////
int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multi-samplings to use. 4 is probably fine
  sf::RenderWindow window(sf::VideoMode(800, 800), "View Example", sf::Style::Default, settings);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    return -1;
  }

  float cell_size = 180.0f;
  int grid_width = 16;

  /// now make a view big enough for only part of the scene - 6 cells
  /// Only that part of the scene will be displayed and it
  /// will fill the window
  float view_size = cell_size * 6;
  sf::View view(sf::FloatRect(0, 0, view_size, view_size));
  // Positioning is always done by reference to the centre
  view.setCenter(sf::Vector2f(cell_size * grid_width / 2, cell_size * grid_width / 2));
  window.setView(view);

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

      // Example: move the view with arrow keys
      if (event.type == sf::Event::KeyPressed) {
        float delta = cell_size / 10;
        if (event.key.code == sf::Keyboard::Left)
          view.move(-delta, 0);
        if (event.key.code == sf::Keyboard::Right)
          view.move(delta, 0);
        if (event.key.code == sf::Keyboard::Up)
          view.move(0, -delta);
        if (event.key.code == sf::Keyboard::Down)
          view.move(0, delta);
      }
    }

    window.clear();
    window.setView(view);  // Update the view
    drawGrid(window, 16, 180.0f, font);
    window.display();
  }

  return 0;
}
