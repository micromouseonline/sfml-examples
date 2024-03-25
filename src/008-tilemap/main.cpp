#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>

/// https://www.sfml-dev.org/tutorials/2.6/graphics-vertex-array.php

class TileMap : public sf::Drawable, public sf::Transformable {
 public:
  bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height) {
    // load the tileset texture
    if (!m_tileset_texture.loadFromFile(tileset)) {
      return false;
    }

    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Triangles);
    m_vertices.resize(width * height * 6);
    int texture_tile_width = (m_tileset_texture.getSize().x / tileSize.x);
    int texture_tile_height = (m_tileset_texture.getSize().y / tileSize.y);

    // populate the vertex array, with two triangles per tile
    for (unsigned int i = 0; i < width; ++i)
      for (unsigned int j = 0; j < height; ++j) {
        // get the current tile number
        int tileNumber = tiles[i + j * width];

        // find its position in the tileset texture
        int tu = tileNumber % texture_tile_width;
        int tv = tileNumber / texture_tile_width;

        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex* triangles = &m_vertices[(i + j * width) * 6];

        // define the 6 corners of the two triangles
        triangles[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
        triangles[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
        triangles[2].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
        triangles[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
        triangles[4].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
        triangles[5].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
        triangles[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
        triangles[2].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
        triangles[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
        triangles[4].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
        triangles[5].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
      }

    return true;
  }

 private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset_texture;

    // draw the vertex array
    target.draw(m_vertices, states);
  }

  sf::VertexArray m_vertices;
  sf::Texture m_tileset_texture;
};

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(600, 400), "008 Tilemap", sf::Style::Default, settings};
  window.setFramerateLimit(60);

  sf::Clock up_time{};

  /// you need the absolute or relative path to the font file.
  sf::Font aw_font;
  if (!aw_font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }

  // define the level with an array of tile indices
  // clang-format off
    const int level[] =
    {
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
        0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
        0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
        0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
        2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
        0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
    };
  // clang-format on
  // create the tilemap from the level definition
  TileMap map;
  if (!map.load("assets/images/tileset.png", sf::Vector2u(32, 32), level, 16, 8)) {
    return -1;
  }
  while (window.isOpen()) {
    /// process all the inputs
    sf::Event event{};
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed: {
          window.close();
          break;
        }
        case sf::Event::Resized: {
          sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
          window.setView(sf::View(visibleArea));  // or everything distorts
          // alternatively only allow fixed aspect ratio resizing
          break;
        }
        default:
          break;
      }
    }

    /// and redraw the window
    window.clear();

    /// sfml knows how to draw a vertex array
    window.draw(map);

    window.display();
  }

  return 0;
}
