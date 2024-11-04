//
// Created by peter on 04/11/24.
//

#ifndef SENSOR_H
#define SENSOR_H

#include "raycaster.h"
#include "utils.h"

/***
 * simulate a single sensor by casting out a fan of rays.
 * @param renderTarget  - the place we will draw the rays
 * @param image - holds the map
 * @param pos  - the position of the emitter
 * @param angle - the angle of the centre of the fan
 * @param width - the angle subtended by the fan
 * @param steps - how many rays to use in the fan
 * @return - the average distance of all the rays
 */

class Sensor {
 public:
  Sensor(sf::Vector2f origin, float angle, float half_angle = 5.0f, int steps = 11)
      : m_origin(origin), m_angle(angle), m_half_angle(half_angle), m_rays(steps) {}

  void set_origin(sf::Vector2f origin) { m_origin = origin; }

  void set_angle(float angle) { m_angle = angle; }

  float draw(sf::RenderTarget& renderTarget, sf::Image& image) {
    float power = 0;
    float ray_angle = m_angle - m_half_angle;
    float inc = 2 * m_half_angle / m_rays;
    sf::Vertex line[] = {sf::Vertex(m_origin), sf::Vertex(m_origin)};
    sf::Color color(128, 0, 128);
    line[0].color = color;
    for (int i = 0; i < m_rays; i++) {
      sf::Vector2f hitPosition = castRay(image, sf::Color::Red, m_origin, ray_angle);
      float d = distance(m_origin, hitPosition);
      float p = (800.0 / d);  // calculate reading
      p = p * p;
      power += p;  // distance(pos, hitPosition);
      line[1].position = hitPosition;
      line[1].color = color - sf::Color(0, 0, 0, d);
      renderTarget.draw(line, 2, sf::Lines);
      ray_angle += inc;
    }
    float average = power / (float)m_rays;
    return std::min(average, 1024.0f);
  }

  static float sensor(sf::RenderTarget& renderTarget, sf::Image& image, sf::Vector2f pos, float angle, float width, int steps) {
    float power = 0;
    float start = angle - width / 2;
    float inc = width / steps;
    sf::Vertex line[] = {sf::Vertex(pos), sf::Vertex(pos)};
    line[0].color = sf::Color(255, 128, 0, 196);
    for (int i = 0; i < steps; i++) {
      sf::Vector2f hitPosition = castRay(image, sf::Color::Red, pos, start);
      float d = distance(pos, hitPosition);
      float p = (800.0 / d);
      p = p * p;
      power += p;  // distance(pos, hitPosition);
      line[1].position = hitPosition;
      line[1].color = sf::Color(255, 128, 0, 255 - d);
      renderTarget.draw(line, 2, sf::Lines);
      start += inc;
    }
    float average = power / (float)steps;
    return std::min(average, 1024.0f);
  }

  sf::Vector2f m_origin = {0, 0};
  int m_rays = 10;
  float m_half_angle = 5.0f;
  float m_angle = 0.0f;
};

#endif  // SENSOR_H
