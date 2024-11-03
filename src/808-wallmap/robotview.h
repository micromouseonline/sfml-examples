//
// Created by peter on 02/11/2024.
//

#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "robot.h"

class RobotView : public sf::Sprite {
 public:
  RobotView(Robot& robot) : m_robot(robot) {
    shape.setSize(sf::Vector2f(10, 10));  // Example size
    shape.setOrigin(5, 5);                // Center the origin for rotation
  }

  bool load_texture(const std::string& filename) {
    if (!m_texture.loadFromFile(filename)) {
      std::cerr << "Unable to load texture\n";
      return false;
    }
    setTexture(m_texture);
    setTextureRect(sf::IntRect(0, 0, m_robot.m_width, m_robot.m_height));
    setOrigin(38.5, 60);
    setPosition(270, 270);
    setRotation(0.0);
    update();
    return true;
  }

  void update() {
    setTextureRect(sf::IntRect(m_robot.m_state * m_robot.m_width, 0, m_robot.m_width, m_robot.m_height));
    setRotation(m_robot.m_angle);
    setPosition(m_robot.m_x, m_robot.m_y);
  }

 private:
  Robot& m_robot;
  sf::RectangleShape shape;
  sf::Texture m_texture;
  float m_width;
  float m_height;
};

#endif  // ROBOTVIEW_H
