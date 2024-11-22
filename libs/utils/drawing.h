//
// Created by peter on 22/11/24.
//

#ifndef DRAWING_H
#define DRAWING_H

#include <SFML/Graphics.hpp>

inline void draw_line(sf::RenderTarget& target, sf::Vector2f pos, sf::Vector2f vec, sf::Color color = sf::Color::White) {
  sf::Vertex line[2];
  line[0].position = {pos.x, pos.y};
  line[0].color = color;
  line[1].position = {vec.x, vec.y};
  line[1].color = color;
  target.draw(line, 2, sf::Lines);
}

#endif  // IMGUI_SFML_STARTER_DRAWING_H
