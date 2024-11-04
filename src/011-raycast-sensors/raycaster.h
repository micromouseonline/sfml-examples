//
// Created by peter on 04/11/24.
//

#ifndef IMGUI_SFML_STARTER_RAYCASTER_H
#define IMGUI_SFML_STARTER_RAYCASTER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "utils.h"
/***
 * A Bresenham style raycast. There may be no performance benefit compared to a
 * linear interpolation solution.
 *
 * casts out a single ray
 *
 * This needs encapsulating in a class
 *
 * @param image - the image holding the map. think of it like an occupancy grid
 * @param origin - the start point for the ray
 * @param angle - the angle of the ray
 * @return - the distance at which it hits the wall colour or runs out of range
 */
inline sf::Vector2f castRay(const sf::Image& image, sf::Color wall_colour, const sf::Vector2f& origin, float angle) {
  angle = angle * (3.14159 / 180);  // Convert angle to radians
  float range = 254;                // the maximum distance we will cast out
  sf::Vector2f rayDirection(cos(angle), sin(angle));
  sf::Vector2f dest = origin + range * rayDirection;
  sf::Color color = getColorAtPixel(image, static_cast<int>(origin.x), static_cast<int>(origin.y));

  // Bresenham's line algorithm for efficient traversal
  int x0 = static_cast<int>(origin.x);
  int y0 = static_cast<int>(origin.y);
  int x1 = static_cast<int>(dest.x);
  int y1 = static_cast<int>(dest.y);

  int dx = std::abs(x1 - x0);
  int dy = std::abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx - dy;

  while (x0 != x1 || y0 != y1) {
    if (!isWithinBounds(image.getSize(), x0, y0)) {
      return {(float)x1, (float)y1};
    }

    color = getColorAtPixel(image, x0, y0);
    if (color == wall_colour) {
      break;
    }

    int err2 = 2 * err;
    if (err2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (err2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
  return {(float)x0, (float)y0};
}

#endif  // IMGUI_SFML_STARTER_RAYCASTER_H
