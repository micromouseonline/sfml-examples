//
// Created by peter on 04/11/24.
//

#ifndef SENSOR_H
#define SENSOR_H

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

const float RADIANS = (3.14159265359f / 180.0f);

class Sensor {
 public:
  Sensor(sf::Vector2f origin, float angle, float half_angle = 5.0f, int steps = 11)
      : m_origin(origin), m_angle(angle), m_half_angle(half_angle), m_rays(steps) {}

  // Returns distance between points
  float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
  }

  void set_origin(sf::Vector2f origin) { m_origin = origin; }

  void set_angle(float angle) {
    m_angle = angle;
    float theta = (float)(angle * M_PI) / 180.0f;
    m_dir = sf::Vector2f(cosf(theta), sinf(theta));
  }

  float draw(sf::RenderTarget& renderTarget, sf::RectangleShape& m_rectangle) {
    float power = 0;

    float ray_angle = m_angle - m_half_angle;

    float inc = 2.0f * m_half_angle / (float)m_rays;

    float start_angle = (m_angle - m_half_angle) * RADIANS;
    float end_angle = (m_angle + m_half_angle) * RADIANS;
    float inc_angle = (end_angle - start_angle) / (float)m_rays;

    sf::Vertex line[] = {sf::Vertex(m_origin), sf::Vertex(m_origin)};
    sf::Color color(128, 0, 128);
    line[0].color = color;
    for (int i = 0; i < m_rays; i++) {
      float theta = (ray_angle + i * inc) * M_PI / 180.0f;
      sf::Vector2f dir = sf::Vector2f(cosf(theta), sinf(theta));

      float angle = start_angle + i * inc_angle;
      dir = sf::Vector2f(std::cos(angle), std::sin(angle));
      float rayLength = 400.0f;

      // Compute the end point of the ray
      sf::Vector2f endPoint = {m_origin.x + rayLength * std::cos(angle), m_origin.y + rayLength * std::sin(angle)};
      float p = test_to_rect(m_rectangle, dir);
      sf::Vector2f hitPosition = m_origin + dir * p;
      line[1].position = hitPosition;
      line[1].color = color - sf::Color(0, 0, 0, 255 - p);
      renderTarget.draw(line, 2, sf::Lines);
      power += p;
    }
    float average = power / (float)m_rays;
    return std::min(average, 1024.0f);
  }

  float test_to_rect(const sf::RectangleShape& rectangle, sf::Vector2f ray_dir, float maxRayLength = 255.0f) {
    // Get the global bounds of the rectangle
    sf::FloatRect bounds = rectangle.getGlobalBounds();
    sf::Vector2f rectMin(bounds.left, bounds.top);
    sf::Vector2f rectMax(bounds.left + bounds.width, bounds.top + bounds.height);

    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    // Check each axis (x and y)
    for (int i = 0; i < 2; ++i) {
      float origin = (i == 0) ? m_origin.x : m_origin.y;
      float dir = (i == 0) ? ray_dir.x : ray_dir.y;
      float min = (i == 0) ? rectMin.x : rectMin.y;
      float max = (i == 0) ? rectMax.x : rectMax.y;

      if (std::abs(dir) < 1e-6) {
        // Ray is parallel to the axis
        if (origin < min || origin > max) {
          return 1300.f;  // No intersection
        }
      } else {
        // Compute intersection times for the axis
        float t1 = (min - origin) / dir;
        float t2 = (max - origin) / dir;

        if (t1 > t2) {
          std::swap(t1, t2);
        }  // Ensure t1 is the near intersection
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax)
          return 200.f;  // No intersection
      }
    }
    // If tmax < 0, the intersection is behind the ray origin
    if (tmax < 0)
      return 200.f;

    // If the nearest intersection is beyond the maximum ray length, return no intersection
    if (tmin > maxRayLength)
      return maxRayLength;

    // Return the nearest positive intersection distance
    return tmin >= 0 ? tmin : tmax;
  }

  sf::Vector2f m_origin = {0, 0};
  sf::Vector2f m_dir = {0, 0};

  int m_rays = 10;
  float m_half_angle = 5.0f;
  float m_angle = 0.0f;
};

#endif  // SENSOR_H
