//
// Created by peter on 02/11/2024.
//

#ifndef ROBOT_H
#define ROBOT_H

#include <cmath>
class Robot {
 public:
  Robot(float width, float height, float origin_x, float origin_y) : m_width(width), m_height(height), m_origin_x(origin_x), m_origin_y(origin_y) {
    setPosition(100, 100);
  };

  void update(float deltaTime) {
    float ds = m_speed * deltaTime;
    float dx = std::cos((m_angle - 90) * 3.14 / 180) * ds;
    float dy = std::sin((m_angle - 90) * 3.14 / 180) * ds;
    m_x += dx;
    m_y += dy;
  }

  void setSpeed(float speed) { m_speed = speed; }
  void setOmega(float omega) { m_omega = omega; }
  void setDirection(float angle) { m_angle = angle; }
  void move(sf::Vector2f direction) {
    m_x += direction.x;
    m_y += direction.y;
  }
  void setPosition(float x, float y) {
    m_x = x;
    m_y = y;
  }
  void rotate(float angle) { m_angle += angle; }
  void set_state(int state) { m_state = state; }

  // Add more methods as needed for behaviour and dynamics

  // private:
  int m_state = 0;  // used to select the section from the texture. Not very efficient
  float m_width = 0;
  float m_height = 0;
  float m_origin_x = 0;
  float m_origin_y = 0;
  float m_x = 0.0f;
  float m_y = 0.0f;      // Position
  float m_angle = 0.0f;  // Angle in radians
  float m_speed = 0.0f;
  float m_omega = 0.f;
};

#endif  // ROBOT_H
