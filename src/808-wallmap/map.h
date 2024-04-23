#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "mazedata.h"
#include "util.h"

// clang-format on
extern int* japan2007ef_maz;
/// https://www.sfml-dev.org/tutorials/2.6/graphics-view.php
/***
 * Assume all mazes are 32x32?
 * We can just display the lower left quadrant for 16x16 mazes?
 *
 */

class TileMap : public sf::Drawable, public sf::Transformable {
 public:
  bool load_textures(const std::string& tileset, sf::Vector2u tileSize) {
    // load the tileset texture
    if (!m_tileset_texture.loadFromFile(tileset)) {
      return false;
    }
    return true;
  }
  bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height) {
    if (!load_textures(tileset, tileSize)) {
      return false;
    }
    for (unsigned int x = 0; x < height; ++x) {
      for (unsigned int y = 0; y < width; ++y) {
        // get the current tile number
        int k = (y) + (x)*width;
        int tileNumber = tiles[k];
        if (x == 1 && y == 0) {
          tileNumber = 15;
        } else {
          tileNumber = 0;
        }
        tileNumber = tiles[k];
        m_map_labels[x][y].setFont(font);
        m_map_labels[x][y].setCharacterSize(45);
        m_map_labels[x][y].setString(std::to_string(10 * k));
        m_map_labels[x][y].setFillColor(sf::Color::Yellow);
        int dx = m_map_labels[x][y].getLocalBounds().width / 2;
        int dy = m_map_labels[x][y].getLocalBounds().height / 1;
        m_map_labels[x][y].setOrigin(sf::Vector2f(dx, dy));

        int txx = x * tileSize.x;
        int tyy = (15 - y) * tileSize.y;

        m_level_map[x][y].setTexture(m_tileset_texture);
        m_level_map[x][y].setPosition(sf::Vector2f(txx, tyy));
        set_tile_type(x, y, tileNumber);
        // m_level_map[x][y].setTextureRect(sf::IntRect(tileNumber * tileSize.x, 0, tileSize.x, tileSize.y));
        if (x == 7 && (y == 7 || y == 8)) {
          m_level_map[x][y].setColor(sf::Color::Red);
        }
        //        std::cout << k << " > " << m_level_map[x][y].getLocalBounds().height << std::endl;

        txx += tileSize.x / 2;
        tyy += tileSize.y / 2;
        m_map_labels[x][y].setPosition(sf::Vector2f(txx, tyy));
      }
    }

    return true;
  }
  void set_tile_type(int x, int y, int type) {  //
    m_level_map[x][y].setTextureRect(sf::IntRect(type * m_tileSize.x, 0, m_tileSize.x, m_tileSize.y));
  }

  void set_font(sf::Font& font) { this->font = font; }

  void clear_colours() {
    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 16; y++) {
        m_level_map[x][y].setColor(sf::Color::White);
      }
    }
  }

  void set_cell_colour(int x, int y, sf::Color colour = sf::Color(0, 0, 0, 255)) {
    if (x < 0 || x > 15) {
      return;
    }
    if (y < 0 || y > 15) {
      return;
    }
    m_level_map[x][y].setColor(colour);
  }

 private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset_texture;

    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 16; x++) {
        target.draw(m_level_map[x][y]);
        target.draw(m_map_labels[x][y]);
      }
    }
  }

  sf::Font font;
  sf::Vector2u m_tileSize = sf::Vector2u(180, 180);
  sf::Sprite m_level_map[16][16];
  sf::Text m_map_labels[16][16];
  sf::Texture m_tileset_texture;
  std::array<sf::RectangleShape, 1089> posts;
  std::array<sf::RectangleShape, 2112> walls;
};