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
 * Simple utility to resize a rectangle by shrinking the borders
 */
void shrink(sf::RectangleShape& rect, float pixels) {
  sf::Vector2f currentSize = rect.getSize();
  sf::Vector2f currentPos = rect.getPosition();
  sf::Vector2f newSize = currentSize - sf::Vector2f(pixels, pixels);
  rect.setSize(newSize);
  sf::Vector2f offset(pixels / 2, pixels / 2);
  rect.setPosition(currentPos + offset);
}

/**
 * Draw a grid onto the supplied render target and label each cell
 * with its x,y coordinates.
 * Return the grid's global bounds
 *
 * It is not important what we draw for this demo. It is just some content
 * that makes it easy to understand which part we are looking at
 */
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
  // prepare to write the x,y coordinate in each cell
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(static_cast<int>(cell_size / 4));
  text.setFillColor(sf::Color::White);

  for (int i = 0; i < cell_count; ++i) {
    for (int j = 0; j < cell_count; ++j) {
      float x = i * cell_size;
      float y = cell_count * cell_size - j * cell_size;
      // draw the cell
      cell.setOrigin(0, cell_size);
      cell.setPosition(x + wall_size, y + wall_size);
      target.draw(cell);
      // and its (x,y) coordinate
      text.setString("(" + std::to_string(i) + "," + std::to_string(j) + ")");
      float text_width = text.getGlobalBounds().width;
      float text_height = text.getGlobalBounds().height;
      text.setPosition(x + (cell_size - text_width) / 2, (y - (cell_size + text_height) / 2));
      target.draw(text);
    }
  }
  return bg.getGlobalBounds();
}

// a quick and dirty function to set up the sprite that will be used for the background grid
sf::Sprite& make_grid(sf::Sprite& sprite) {
  return sprite;
}

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  uint32_t window_style = sf::Style::Titlebar | sf::Style::Close;  // no resizing
  sf::RenderWindow window(sf::VideoMode(800, 800), WINDOW_TITLE, window_style, settings);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/consolas.ttf")) {
    return -1;
  }

  ////////////////////////////////////////////////////////////////////////////
  /// set up the scene background

  /// Work out how big the grid will be. Assume 1 pixel == 1 mm
  float wall_size = 12.0f;
  float cell_size = 180.0f;
  int grid_width = 16;
  float grid_size = grid_width * cell_size + wall_size;

  /// create a render texture that the grid will be drawn onto.
  /// attempts to draw outside this texture will be cropped
  sf::RenderTexture grid_texture;
  grid_texture.create(grid_size, grid_size);

  /// The grid view is static so we can just draw it once here
  grid_texture.clear();
  drawGrid(grid_texture, grid_width, cell_size, wall_size, font);
  grid_texture.display();

  /// And assign it to a sprite
  sf::Sprite grid(grid_texture.getTexture());

  ///////////////////////////////////////////////////////////////////////////
  ///
  /// Now create two views of the scene
  /// Both will see the same amount of the scene and be centred on the same spot
  sf::Vector2f centre(grid_texture.getSize().x / 2, grid_texture.getSize().y / 2);
  sf::Vector2f size = sf::Vector2f(cell_size * 4 + wall_size, cell_size * 4 + wall_size);

  sf::View view1(centre, size);
  sf::View view2(centre, size);

  /// but their viewports in the window will be different:

  /// SFML Viewports are _proportions_ of the screen in the range 0..1 that
  /// a view will be rendered in.
  /// The view will be stretched or squashed as needed to fill the viewport.
  /// Ensure that the aspect ratio of the viewport matches the aspect ratio
  /// of the view or there will be some distortion.
  /// When setting a viewport, you give a rectangle which holds
  /// the upper left corner and the width and height.
  /// Remember that these are proportions, not extents. So, if you wanted to
  /// create a viewport that took the entire left quarter of the window, the
  /// declaration would be
  ///    view.setViewport(sf::FloatRect(0.f, 0.f, 0.25f, 1.0f));

  /// Here, the viewports are the top left and bottom right quarters of the
  /// window
  view1.setViewport(sf::FloatRect(0.f, 0.f, 0.5f, 0.5f));
  view2.setViewport(sf::FloatRect(0.5f, 0.5f, 0.5f, 0.5f));

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // views can be moved, rotated, scaled etc. independently

      // Zoom view1 with mouse wheel
      if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
          view1.zoom(15.0f / 16.0f);  // Zoom in
        } else {
          view1.zoom(16.0 / 15.0);  // Zoom out
        }
      }

      // Move view2 with arrow keys
      if (event.type == sf::Event::KeyPressed) {
        bool shift_key = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));
        float delta = shift_key ? cell_size : wall_size;
        if (event.key.code == sf::Keyboard::Left)
          view2.move(-delta, 0);
        if (event.key.code == sf::Keyboard::Right)
          view2.move(delta, 0);
        if (event.key.code == sf::Keyboard::Up)
          view2.move(0, -delta);
        if (event.key.code == sf::Keyboard::Down) {
          view2.move(0, delta);
        }
      }
    }

    window.clear();

    // Each view must be drawn separately even though they are views of the same scene.
    // Not sure why.
    window.setView(view2);
    window.draw(grid);

    window.setView(view1);
    window.draw(grid);

    window.display();
  }

  return 0;
}
